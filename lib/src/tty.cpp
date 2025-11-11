#include "tty.hpp"
// TODO: Posix support

#include <cstdio>

#if defined(_WIN32)
#define ESC "\x1b"
#define CSI ESC "["
#endif

namespace Basedline {

bool TTY::enableRaw () {
	if (raw) return true;
#if defined(_WIN32)
/*
	DWORD rawInMode = hInModeSave & ~(
		ENABLE_ECHO_INPUT |
		ENABLE_LINE_INPUT |
		ENABLE_PROCESSED_INPUT
	);
*/
	DWORD rawInMode = hInModeSave;
	DWORD rawOutMode = hOutModeSave |
		ENABLE_PROCESSED_OUTPUT |
		ENABLE_WRAP_AT_EOL_OUTPUT |
		ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	return SetConsoleMode (hIn, rawInMode) & SetConsoleMode (hOut, rawOutMode);
#endif
}

bool TTY::disableRaw () {
	if (!raw) return true;
#if defined(_WIN32)
	return SetConsoleMode (hIn, hInModeSave) & SetConsoleMode (hOut, hOutModeSave);
#endif
}

bool TTY::setRaw (bool raw) {
	bool isSet = raw ? enableRaw() : disableRaw();
	this->raw = raw;
	return isSet;
}

void TTY::ctrl (TTY::Input& input, bool& skip) {
	skip = false;
#if defined(_WIN32)
	switch (input.vkey) {
		// Handle Ctrl-D and Ctrl-Z
		case 'D':
		case 'Z':
			input.flags[Input::Flags::IS_EOF] = true;
			break;
		// No other key
		case VK_CONTROL:
			skip = true;
			break;
		default: break;
	}
#endif
}

void TTY::backspace () {
	std::printf (CSI "0M");
}

TTY::Input TTY::getc () {
	Input ret;
#if defined(_WIN32)
	while (true) {
		INPUT_RECORD input;
		DWORD input_count;
		bool skip = false;

		if (!ReadConsoleInput(hIn, &input, 1, &input_count))
			return Input::make_err();
		if (input_count != 1)
			return Input::make_err();
		if (input.EventType != KEY_EVENT)
			return Input::make_err();
		if (!input.Event.KeyEvent.bKeyDown)
			continue;
		
		ret.ch = input.Event.KeyEvent.uChar.AsciiChar;
		ret.vkey = input.Event.KeyEvent.wVirtualKeyCode;
		ret.flags[Input::Flags::OK] = true;

		DWORD mods = input.Event.KeyEvent.dwControlKeyState;
		ret.flags[Input::Flags::HAS_CTRL] = mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);
		if (ret.flags[Input::Flags::HAS_CTRL])
			ctrl (ret, skip);
		if (skip)
			continue;

		std::fprintf(stderr, "virt 0x%04x, chr 0x%04x ('%c') %s \n",
			ret.vkey, ret.ch, ret.ch,
			ret.flags[Input::Flags::HAS_CTRL] ? "CTRL" : "");

		return ret;
	}
#endif
}

void TTY::putc (int c) {
	std::fputc (c, stdout);
}

TTY::TTY () {
#if defined(_WIN32)
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hIn, &hInModeSave);
	GetConsoleMode (hOut, &hOutModeSave);
#endif
}

}
