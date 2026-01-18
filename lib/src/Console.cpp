#include "Console.hpp"

#include <cstdio>
#include <format>
#include <stdexcept>

#include "Debug.hpp"
#include "VT.hpp"

#if defined(_WIN32)
# define IF_CONPTY if (conpty)
#else
# define IF_CONPTY if (true)
#endif

namespace Basedline {

// Current: 2 per print, 4 on no events
#if defined(BASEDLINE_DEBUG) && defined(_WIN32)
size_t csbiCalls = 0;
#endif

coord_t Console::Cursor::pos () {
#if defined(_WIN32)
	return con.csbi().dwCursorPosition;
#endif
}

// TODO: trace moves, find excess ones
void Console::Cursor::move (coord_t pos) {
	IF_CONPTY {
		std::printf (VT_CUP, pos.Y + 1, pos.X + 1);
	} else {
#if defined(_WIN32)
		if (!SetConsoleCursorPosition (con.hOut, pos)) [[unlikely]]
			throw std::runtime_error (std::format ("Can't move cursor for hOut {} to [{} {}]", con.hOut, pos.X, pos.Y));
#endif
	}
}

void Console::Cursor::shift (termsize_t dx) {
	if (dx == 0) [[unlikely]] return;
	coord_t currPos = pos();
	move (wrap (currPos.Y, currPos.X + dx));
}

coord_t Console::Cursor::wrap (termsize_t line, ssize_t pos) {
	termsize_t lineWidth = con.line_width();
	termsize_t Y = static_cast<termsize_t> (line + pos / lineWidth);
	termsize_t X = static_cast<termsize_t> (pos % lineWidth);
	if (X < 0) [[unlikely]]
		return {static_cast<termsize_t> (X + lineWidth), static_cast<termsize_t> (Y - 1)};
	else if (X > 0 && con.is_last_column (X - 1)) [[unlikely]]
		return {0, static_cast<termsize_t> (Y + 1)};
	else return {X, Y};
}

#if defined(_WIN32)
CONSOLE_SCREEN_BUFFER_INFO Console::csbi () {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo (hOut, &csbi)) [[unlikely]]
		throw std::runtime_error (std::format ("Can't get CSBI for Console hOut {}", hOut));
# if defined(BASEDLINE_DEBUG)
	csbiCalls++;
# endif
	return csbi;
}
#endif

bool Console::configure () {
	if (configured) return true;
#if defined(_WIN32)
	DWORD outMode = hOutModeSave |
		ENABLE_PROCESSED_OUTPUT |
		ENABLE_WRAP_AT_EOL_OUTPUT;
	bool vt = hOutModeSave & ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	IF_CONPTY {
		if (!vt)
			outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	} else {
		if (vt)
			outMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	}
	configured = SetConsoleMode (hOut, outMode);
#endif
	return configured;
}

bool Console::unconfigure () {
	if (!configured) return true;
#if defined(_WIN32)
	configured = !SetConsoleMode (hOut, hOutModeSave);
	return !configured;
#endif
}

void Console::refresh_size () {
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi = this->csbi();
	conSize = csbi.dwSize;
	IF_CONPTY botLine = csbi.srWindow.Bottom;
#endif
}

bool Console::has_input () {
#if defined(_WIN32)
	DWORD input_events = 0;
	if (!GetNumberOfConsoleInputEvents (hIn, &input_events))
		return false;
	return (input_events > 0);
#else
	// select
#endif
}

RawInput Console::get_raw_input () {
	RawInput rawInput;
#if defined(_WIN32)
	INPUT_RECORD inputRec;
	DWORD inputCount;

	if (!ReadConsoleInput (hIn, &inputRec, 1, &inputCount) || inputCount != 1)
		return rawInput;

	switch (inputRec.EventType) {
		[[likely]] case KEY_EVENT:
			if (!inputRec.Event.KeyEvent.bKeyDown) break;
			rawInput.type = RawInput::Type::Key;
			rawInput.ev.key = {
				.ch   = inputRec.Event.KeyEvent.uChar.AsciiChar,
				.vkey = inputRec.Event.KeyEvent.wVirtualKeyCode,
				.mods = inputRec.Event.KeyEvent.dwControlKeyState
			};
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
			rawInput.type = RawInput::Type::Resize;
			rawInput.ev.newSize = inputRec.Event.WindowBufferSizeEvent.dwSize;
			break;
		default: break;
	}

#endif
	return rawInput;
}

void Console::putc (int c) {
	std::fputc (c, stdout);
}

void Console::puts (std::string_view s) {
	std::fputs (s.data(), stdout);
}

void Console::clear_chars (size_t count) {
	if (count == 0) [[unlikely]] return;
	static std::string spaces;
	if (count > spaces.length())
		spaces.append (count - spaces.length(), ' ');
	std::fwrite (spaces.data(), 1, count, stdout);
}

void Console::clear_lines (termsize_t from, termsize_t linesToClear) {
	if (linesToClear <= 0) [[unlikely]] return;
	if (from < 0) [[unlikely]] from = 0;
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi = this->csbi();
	coord_t scrBufSize = csbi.dwSize;
	if (from + linesToClear > scrBufSize.Y) [[unlikely]] linesToClear = scrBufSize.Y - from;
#endif

	IF_CONPTY {
		std::printf (VT_DECSC VT_CUP VT_DL VT_DECSR, from + 1, 1, linesToClear);
	} else {
#if defined(_WIN32)
		coord_t startPos = {0, from};
		DWORD charsToWrite = linesToClear * scrBufSize.X;
		DWORD written;
		FillConsoleOutputCharacter (hOut, ' ', charsToWrite, startPos, &written);
		FillConsoleOutputAttribute (hOut, csbi.wAttributes, charsToWrite, startPos, &written);
#endif
	}
}

termsize_t Console::bottom_line () {
	IF_CONPTY {
		return botLine;
	} else {
#if defined(_WIN32)
		return csbi().srWindow.Bottom;
#endif
	}
}

termsize_t Console::line_width () {
	return conSize.X;
}

void Console::scroll (termsize_t linesToScroll) {
	if (linesToScroll <= 0) [[unlikely]] return;
	IF_CONPTY std::printf (VT_SU VT_CUU, linesToScroll, linesToScroll);
	// For CMD no explicit scroll is required
}

bool Console::is_last_column (termsize_t x) {
	return x >= line_width() - 1;
}

Console::Console () : cursor (*this) {
#if defined(_WIN32)
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hOut, &hOutModeSave);
	BL_DEBUG ("New Console hOut {} hIn {}\n", hOut, hIn);

	// For some unknown reason, Microslop did not provide a documented way to check if we are being run
	// in a ConPTY terminal, so use buffer height = window height check as it seems to work for now
	CONSOLE_SCREEN_BUFFER_INFO csbi = this->csbi();
	conpty = (csbi.dwSize.Y == csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
	BL_DEBUG ("ConPTY: {}\n", conpty);

	conSize = csbi.dwSize;
	IF_CONPTY botLine = csbi.srWindow.Bottom;
#endif
}

}
