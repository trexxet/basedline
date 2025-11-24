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

#if defined(_WIN32)
CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer::csbi () {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo (hOut, &csbi);
	return csbi;
}
#endif

bool ConsoleBuffer::enable_raw () {
	if (raw) return true;
#if defined(_WIN32)
	DWORD rawInMode = hInModeSave;
	DWORD rawOutMode = hOutModeSave |
		ENABLE_PROCESSED_OUTPUT |
		ENABLE_WRAP_AT_EOL_OUTPUT;
	raw = (SetConsoleMode (hIn, rawInMode) & SetConsoleMode (hOut, rawOutMode));
	return raw;
#endif
}

bool ConsoleBuffer::disable_raw () {
	if (!raw) return true;
#if defined(_WIN32)
	raw = !(SetConsoleMode (hIn, hInModeSave) & SetConsoleMode (hOut, hOutModeSave));
	return !raw;
#endif
}

ConsoleBuffer::ConsoleBuffer () {
#if defined(_WIN32)
	// TODO: is hIn ever needed?
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hIn, &hInModeSave);
	GetConsoleMode (hOut, &hOutModeSave);
#endif
}

void Cursor::save () {
#if defined(_WIN32)
	pos[currType] = tty.csbi().dwCursorPosition;
#endif
}

void Cursor::set (Cursor::Type type) {
	if (type == currType) return;
	save();
	currType = type;
	move (pos[type]);
}

void Cursor::move (coord_t pos) {
#if defined(_WIN32)
	SetConsoleCursorPosition (tty.hOut, pos);
#endif
}

Cursor::Cursor (ConsoleBuffer& tty) : tty (tty) {
	currType = Type::CurOutput;
	save();
	pos[Type::CurInput] = pos[Type::CurOutput];
}

void TTY::TTYCursor::input_shift (termsize_t dx, termsize_t left_constraint) {
	set (Type::CurInput);
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi = tty.csbi();
	termsize_t x = csbi.dwCursorPosition.X + dx;
	if (x < left_constraint) return;
	move ({x, csbi.dwCursorPosition.Y});
#endif
}

void TTY::TTYCursor::input_move_down () {
	set (Type::CurInput);
#if defined(_WIN32)
	termsize_t inputLineY = tty.csbi().srWindow.Bottom;
#endif
	move ({0, inputLineY});
}

bool TTY::set_raw (bool raw) {
	return raw ? enable_raw() : disable_raw();
}

void TTY::set_prompt (const std::string& prompt) {
	this->prompt = prompt;
	// TODO: check if prompt.length fits in term
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
			cursor.input_shift (-1, prompt.length());
			break;
		case VK_RIGHT:
			input.flags[Input::Flags::IS_RIGHT] = true;
			cursor.input_shift (1, prompt.length());
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

void TTY::putc (int c, Cursor::Type curType) {
	cursor.set (curType);
	std::fputc (c, stdout);
}

void TTY::puts (const std::string& s, Cursor::Type curType) {
	cursor.set (curType);
	std::printf (s.c_str());
}

}
