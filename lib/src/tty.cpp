#include "tty.hpp"
// TODO: Posix support

#include <cstdio>

#define CSI "[\x1b"

namespace Basedline {

#if defined(BASEDLINE_DEBUG) && 0
#define bl_debug(fmt, ...) { std::fprintf(stderr, fmt, ##__VA_ARGS__); }
#else
#define bl_debug(fmt, ...)
#endif

void TTY::Cursor::shift (short dx) {
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (!GetConsoleScreenBufferInfo (tty.hOut, &info))
		return;
	short x = info.dwCursorPosition.X + dx;
	if (x < bounds.xMin) return;
	move (x, info.dwCursorPosition.Y);
#endif
}

void TTY::Cursor::move (short x, short y) {
#if defined(_WIN32)
	if (SetConsoleCursorPosition (tty.hOut, {x, y}))
		pos = {x, y};
#endif
}


bool TTY::enable_raw () {
	if (raw) return true;
#if defined(_WIN32)
	DWORD rawInMode = hInModeSave;
	DWORD rawOutMode = hOutModeSave |
		ENABLE_PROCESSED_OUTPUT |
		ENABLE_WRAP_AT_EOL_OUTPUT;
	return SetConsoleMode (hIn, rawInMode) & SetConsoleMode (hOut, rawOutMode);
#endif
}

bool TTY::disable_raw () {
	if (!raw) return true;
#if defined(_WIN32)
	return SetConsoleMode (hIn, hInModeSave) & SetConsoleMode (hOut, hOutModeSave);
#endif
}

bool TTY::set_raw (bool raw) {
	bool isSet = raw ? enable_raw() : disable_raw();
	this->raw = raw;
	return isSet;
}

void TTY::set_prompt (std::string_view prompt) {
	this->prompt = prompt;
	// TODO: check if prompt.length fits in term
	cursor.bounds.xMin = prompt.length();
}

bool TTY::ctrl (TTY::Input& input) {
#if defined(_WIN32)
	switch (input.vkey) {
		// Handle Ctrl-D and Ctrl-Z
		case 'D':
		case 'Z':
			input.flags[Input::Flags::IS_EOF] = true;
			break;
		// No other key
		case VK_CONTROL: return true;
		default: break;
	}
#endif
	return false;
}

void TTY::left_right (TTY::Input& input) {
#if defined(_WIN32)
	switch (input.vkey) {
		case VK_LEFT:
			input.flags[Input::Flags::IS_LEFT] = true;
			cursor.shift (-1);
			break;
		case VK_RIGHT:
			input.flags[Input::Flags::IS_RIGHT] = true;
			cursor.shift (1);
			break;
	}
#endif
}

void TTY::backspace () {
	std::printf (CSI "0M");
}

TTY::Input TTY::getc () {
	Input input;
#if defined(_WIN32)
	while (true) {
		INPUT_RECORD inputRec;
		DWORD inputCount;
		bool skip = false;

		if (!ReadConsoleInput(hIn, &inputRec, 1, &inputCount)
		    || inputCount != 1
		    || inputRec.EventType != KEY_EVENT) {
			return Input::make_err();
		}
		if (!inputRec.Event.KeyEvent.bKeyDown)
			continue;
		
		input.ch = inputRec.Event.KeyEvent.uChar.AsciiChar;
		input.vkey = inputRec.Event.KeyEvent.wVirtualKeyCode;
		input.flags[Input::Flags::OK] = true;

		// Ctrl
		DWORD mods = inputRec.Event.KeyEvent.dwControlKeyState;
		input.flags[Input::Flags::HAS_CTRL] = mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);
		if (input.flags[Input::Flags::HAS_CTRL] && ctrl (input))
			continue;

		// Arrows
		if (input.vkey == VK_LEFT || input.vkey == VK_RIGHT)
			left_right (input);

		bl_debug ("%s mods 0x%04x virt 0x%04x chr 0x%04x ('%c') \n",
		          input.flags[Input::Flags::HAS_CTRL] ? "CTRL" : "",
		          mods, input.vkey, input.ch, input.ch);

		return input;
	}
#endif
}

void TTY::putc (int c) {
	std::fputc (c, stdout);
}

TTY::TTY () : cursor (*this) {
#if defined(_WIN32)
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hIn, &hInModeSave);
	GetConsoleMode (hOut, &hOutModeSave);
#endif
}

}
