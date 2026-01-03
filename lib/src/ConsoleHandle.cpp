#include "ConsoleHandle.hpp"

#include <cstdio>
#include <format>
#include <stdexcept>

#include "Debug.hpp"
#include "VT.hpp"

#if defined(_WIN32)
# define IF_VT if (vt)
#else
# define IF_VT if (true)
#endif

#define TRY_VT 1

namespace Basedline {

coord_t ConsoleHandle::Cursor::pos () {
#if defined(_WIN32)
	return con.csbi().dwCursorPosition;
#endif
}

void ConsoleHandle::Cursor::move (coord_t pos) {
	IF_VT {
		std::printf (VT_CUP, pos.Y + 1, pos.X + 1);
	} else {
#if defined(_WIN32)
		if (!SetConsoleCursorPosition (con.hOut, pos)) [[unlikely]]
			throw std::runtime_error (std::format ("Can't move cursor for hOut {} to [{} {}]", con.hOut, pos.X, pos.Y));
#endif
	}
}

// TODO: multiline wrap
void ConsoleHandle::Cursor::shift (termsize_t dx) {
	IF_VT {
		if (dx == 0) return;
		std::printf (dx > 0 ? VT_CUF : VT_CUB, dx > 0 ? dx : -dx);
	} else {
#if defined(_WIN32)
		coord_t curr_pos = pos();
		termsize_t x = curr_pos.X + dx;
		move ({x, curr_pos.Y});
#endif
	}
}

// TODO: if posix/conpty, not if vt
void ConsoleHandle::Cursor::new_input_line (termsize_t *line) {
	IF_VT std::printf ("\n");
	else if (line) [[likely]] (*line)++;
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
	if (TRY_VT) {
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

ConsoleHandle::RawInput ConsoleHandle::get_input () {
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
	IF_VT {
		std::fputc (c, stdout);
	} else {
#if defined(_WIN32)
		if (c == EOF) return;
		char ch = (char) c; 
		WriteConsole (hOut, &ch, 1, NULL, NULL);
#endif
	}
}

void ConsoleHandle::puts (const std::string& s) {
	IF_VT {
		std::printf (s.c_str());
	} else {
#if defined(_WIN32)
		WriteConsole (hOut, s.c_str(), s.length(), NULL, NULL);
#endif
	}
}

void ConsoleHandle::clear_lines (termsize_t from, termsize_t linesToClear) {
	if (linesToClear <= 0) [[unlikely]] return;
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

bool ConsoleHandle::is_last_column (termsize_t x) {
#if defined(_WIN32)
	return x >= csbi().dwSize.X - 1;
#endif
}

ConsoleHandle::ConsoleHandle () : cursor (*this) {
#if defined(_WIN32)
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hOut, &hOutModeSave);
	BL_DEBUG ("New ConsoleHandle hOut {} hIn {}\n", hOut, hIn);
#endif
}

}
