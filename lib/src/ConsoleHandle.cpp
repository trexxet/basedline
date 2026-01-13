#include "ConsoleHandle.hpp"

#include <cstdio>
#include <format>
#include <stdexcept>

#include "Debug.hpp"
#include "VT.hpp"

#if defined(_WIN32)
# define IF_VT if (vt)
# define IF_CONPTY if (conpty)
#else
# define IF_VT if (true)
# define IF_CONPTY if (true)
#endif

#define TRY_VT 1

namespace Basedline {

coord_t ConsoleHandle::Cursor::pos () {
#if defined(_WIN32)
	return con.csbi().dwCursorPosition;
#endif
}

// TODO: trace moves, find excess ones
void ConsoleHandle::Cursor::move (coord_t pos) {
	// For some unknown reason, Microslop's cmd won't move cursor up
	// if it's on the bottom of srWindow even when VT is enabled
	IF_CONPTY {
		std::printf (VT_CUP, pos.Y + 1, pos.X + 1);
	} else {
#if defined(_WIN32)
		if (!SetConsoleCursorPosition (con.hOut, pos)) [[unlikely]]
			throw std::runtime_error (std::format ("Can't move cursor for hOut {} to [{} {}]", con.hOut, pos.X, pos.Y));
#endif
	}
}

void ConsoleHandle::Cursor::shift (termsize_t dx) {
	if (dx == 0) [[unlikely]] return;
	coord_t currPos = pos();
	move (wrap (currPos.Y, currPos.X + dx));
}

coord_t ConsoleHandle::Cursor::wrap (termsize_t line, ssize_t pos) {
#if defined(_WIN32)
	termsize_t lineWidth = con.csbi().dwSize.X;
#endif
	termsize_t Y = static_cast<termsize_t> (line + pos / lineWidth);
	termsize_t X = static_cast<termsize_t> (pos % lineWidth);
	if (X < 0) [[unlikely]]
		return {static_cast<termsize_t> (X + lineWidth), static_cast<termsize_t> (Y - 1)};
	else if (X > 0 && con.is_last_column (X - 1)) [[unlikely]]
		return {0, static_cast<termsize_t> (Y + 1)};
	else return {X, Y};
}

// TODO: cache csbi
#if defined(_WIN32)
CONSOLE_SCREEN_BUFFER_INFO ConsoleHandle::csbi () {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo (hOut, &csbi)) [[unlikely]]
		throw std::runtime_error (std::format ("Can't get CSBI for ConsoleHandle hOut {}", hOut));
	return csbi;
}
#endif

bool ConsoleHandle::configure () {
	if (configured) return true;
#if defined(_WIN32)
	DWORD outMode = hOutModeSave |
		ENABLE_PROCESSED_OUTPUT |
		ENABLE_WRAP_AT_EOL_OUTPUT;
	IF_CONPTY {
		vt = true;
	} else if (TRY_VT) {
		outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		vt = SetConsoleMode (hOut, outMode);
		BL_DEBUG ("VT mode: {}\n", vt);
		if (vt)
			return configured = true;
		outMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	}
	BL_DEBUG ("VT mode: {}\n", vt);
	configured = SetConsoleMode (hOut, outMode);
#endif
	return configured;
}

bool ConsoleHandle::unconfigure () {
	if (!configured) return true;
#if defined(_WIN32)
	configured = !SetConsoleMode (hOut, hOutModeSave);
	return !configured;
#endif
}

bool ConsoleHandle::has_input () {
#if defined(_WIN32)
	DWORD input_events = 0;
	if (!GetNumberOfConsoleInputEvents (hIn, &input_events))
		return false;
	return (input_events > 0);
#else
	// select
#endif
}

RawInput ConsoleHandle::get_raw_input () {
	RawInput rawInput;
#if defined(_WIN32)
	INPUT_RECORD inputRec;
	DWORD inputCount;

	if (!ReadConsoleInput (hIn, &inputRec, 1, &inputCount) || inputCount != 1)
		return rawInput;
	if (inputRec.EventType != KEY_EVENT || !inputRec.Event.KeyEvent.bKeyDown)
		return rawInput;

	rawInput.type = RawInput::Type::Key;
	rawInput.ch = inputRec.Event.KeyEvent.uChar.AsciiChar;
	rawInput.vkey = inputRec.Event.KeyEvent.wVirtualKeyCode;
	rawInput.mods = inputRec.Event.KeyEvent.dwControlKeyState;
#endif
	return rawInput;
}

void ConsoleHandle::putc (int c) {
	std::fputc (c, stdout);
}

void ConsoleHandle::puts (std::string_view s) {
	std::fputs (s.data(), stdout);
}

void ConsoleHandle::clear_chars (size_t count) {
	if (count == 0) [[unlikely]] return;
	IF_VT {
		static std::string spaces;
		if (count > spaces.length())
			spaces.append (count - spaces.length(), ' ');
		std::fwrite (spaces.data(), 1, count, stdout);
	} else {
#if defined(_WIN32)
		CONSOLE_SCREEN_BUFFER_INFO csbi = this->csbi();
		coord_t startPos = cursor.pos();
		DWORD written;
		FillConsoleOutputCharacter (hOut, ' ', count, startPos, &written);
		FillConsoleOutputAttribute (hOut, csbi.wAttributes, count, startPos, &written);
#endif
	}
}

void ConsoleHandle::clear_lines (termsize_t from, termsize_t linesToClear) {
	if (linesToClear <= 0) [[unlikely]] return;
	if (from < 0) [[unlikely]] from = 0;
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi = this->csbi();
	coord_t scrBufSize = csbi.dwSize;
	if (from + linesToClear > scrBufSize.Y) [[unlikely]] linesToClear = scrBufSize.Y - from;
#endif

	IF_VT {
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

// TODO: cache window size
termsize_t ConsoleHandle::bottom_line () {
#if defined(_WIN32)
	return csbi().srWindow.Bottom;
#endif
}

void ConsoleHandle::scroll (termsize_t linesToScroll) {
	if (linesToScroll <= 0) [[unlikely]] return;
	IF_CONPTY std::printf (VT_SU VT_CUU, linesToScroll, linesToScroll);
}

// TODO: cache window size
bool ConsoleHandle::is_last_column (termsize_t x) {
#if defined(_WIN32)
	return x >= csbi().dwSize.X - 1;
#endif
}

void ConsoleHandle::resolve_io_line_overlap (termsize_t& iline, termsize_t& oline) {
	IF_CONPTY {
		oline--;
		std::putchar ('\n');
	} else {
		iline++;
		IF_VT std::putchar ('\n');
	}
}

void ConsoleHandle::scroll_to_fit_text (termsize_t& startLine, termsize_t& lineHeight, size_t len) {
	termsize_t newHeight = cursor.wrap (startLine, len).Y - startLine + 1;
	if (newHeight <= lineHeight) [[likely]] return;

	termsize_t linesToScroll = newHeight - lineHeight;
	scroll (linesToScroll);
	lineHeight = newHeight;
	IF_CONPTY startLine -= linesToScroll;
}

ConsoleHandle::ConsoleHandle () : cursor (*this) {
#if defined(_WIN32)
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hOut, &hOutModeSave);
	BL_DEBUG ("New ConsoleHandle hOut {} hIn {}\n", hOut, hIn);

	// For some unknown reason, Microslop did not provide a documented way to check if we are being run
	// in a ConPTY terminal, so use buffer height = window height check as it seems to work for now
	CONSOLE_SCREEN_BUFFER_INFO csbi = this->csbi();
	conpty = (csbi.dwSize.Y == csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
	BL_DEBUG ("ConPTY: {}\n", conpty);
#endif

	printPos = cursor.pos();
}

}
