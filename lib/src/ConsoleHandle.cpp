#include "ConsoleHandle.hpp"

#include <cstdio>
#include <format>
#include <stdexcept>

#include "Debug.hpp"

namespace Basedline {

#if defined(_WIN32)
CONSOLE_SCREEN_BUFFER_INFO ConsoleHandle::csbi () {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo (hOut, &csbi)) [[unlikely]]
		throw std::runtime_error (std::format ("Can't get CSBI for ConsoleHandle hOut {}", hOut));
	return csbi;
}
#endif

bool ConsoleHandle::enable_raw () {
	if (raw) return true;
#if defined(_WIN32)
	DWORD rawOutMode = hOutModeSave |
		ENABLE_PROCESSED_OUTPUT |
		ENABLE_WRAP_AT_EOL_OUTPUT;
	raw = SetConsoleMode (hOut, rawOutMode);
	return raw;
#endif
}

bool ConsoleHandle::disable_raw () {
	if (!raw) return true;
#if defined(_WIN32)
	raw = !SetConsoleMode (hOut, hOutModeSave);
	return !raw;
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
#if defined(_WIN32)
	if (c == EOF) return;
	char ch = (char) c; 
	WriteConsole (hOut, &ch, 1, NULL, NULL);
#else
	std::fputc (c, stdout);
#endif
}

void ConsoleHandle::puts (const std::string& s) {
#if defined(_WIN32)
	WriteConsole (hOut, s.c_str(), s.length(), NULL, NULL);
#else
	std::printf (s.c_str());
#endif
}

void ConsoleHandle::clear_lines (termsize_t begin, termsize_t end) {
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi = this->csbi();
	coord_t scrBufSize = csbi.dwSize;
	if (end >= scrBufSize.Y) [[unlikely]] end = scrBufSize.Y - 1;

	coord_t startPos = {0, begin};
	DWORD charsToWrite = (end - begin + 1) * scrBufSize.X;
	DWORD written;
	FillConsoleOutputCharacter (hOut, ' ', charsToWrite, startPos, &written);
	FillConsoleOutputAttribute (hOut, csbi.wAttributes, charsToWrite, startPos, &written);
#endif
}

termsize_t ConsoleHandle::bottom_line () {
#if defined(_WIN32)
	return csbi().srWindow.Bottom;
#endif
}

ConsoleHandle::ConsoleHandle () {
#if defined(_WIN32)
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hOut, &hOutModeSave);
	BL_DEBUG ("New ConsoleHandle hOut {} hIn {}\n", hOut, hIn);
#endif
}

}
