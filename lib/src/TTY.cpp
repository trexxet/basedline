#include "TTY.hpp"

#include <cstdio>

#include "Debug.hpp"

#define CSI "[\x1b"

namespace Basedline {

bool TTY::set_raw (bool raw) {
	return raw ? con.enable_raw() : con.disable_raw();
}

void TTY::ctrl (TTY::Input& input) {
	switch (input.vkey) {
		// Handle Ctrl-D and Ctrl-Z
		case 'D':
		case 'Z':
			input.flags[Input::Flags::IS_EOL] = true;
			break;
		default: break;
	}
}

void TTY::left_right (TTY::Input& input) {
	switch (input.vkey) {
		case VK_LEFT:
			input.flags[Input::Flags::IS_LEFT] = true;
			cursor.input_shift (-1);
			break;
		case VK_RIGHT:
			input.flags[Input::Flags::IS_RIGHT] = true;
			cursor.input_shift (1);
			break;
	}
}

TTY::Input TTY::getc () {
	Input input;
#if defined(_WIN32)
	INPUT_RECORD inputRec;
	DWORD inputCount;
	bool skip = false;

	if (!ReadConsoleInput (con.hIn, &inputRec, 1, &inputCount) || inputCount != 1)
		return Input::make_err();
	if (inputRec.EventType != KEY_EVENT || !inputRec.Event.KeyEvent.bKeyDown)
		return input;
	
	input.ch = inputRec.Event.KeyEvent.uChar.AsciiChar;
	input.vkey = inputRec.Event.KeyEvent.wVirtualKeyCode;
	input.flags[Input::Flags::OK] = true;

	// Ctrl
	DWORD mods = inputRec.Event.KeyEvent.dwControlKeyState;
	input.flags[Input::Flags::HAS_CTRL] = mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);
	if (input.flags[Input::Flags::HAS_CTRL]) {
		ctrl (input);
		return input;
	}

	// Arrows
	if (input.vkey == VK_LEFT || input.vkey == VK_RIGHT)
		left_right (input);

	// Enter
	if (input.vkey == VK_RETURN)
		input.flags[Input::Flags::IS_EOL] = true;

	BL_DEBUG ("input {} mods 0x{:04x} virt 0x{:04x} chr 0x{:04x} ('{}')\n",
				input.flags[Input::Flags::HAS_CTRL] ? "CTRL" : "",
				mods, input.vkey, input.ch, 
				std::isprint (input.ch) ? input.ch : ' ');
#endif
	return input;
}

bool TTY::has_input () {
	return con.has_input();
}

void TTY::putc (int c, Cursor::Type curType) {
	cursor.set (curType);
	con.putc (c);
}

void TTY::puts (const std::string& s, Cursor::Type curType) {
	cursor.set (curType);
	con.puts (s);
}

void TTY::clear_lines (termsize_t begin, termsize_t end) {
	if (begin > end) [[unlikely]] return;
	if (begin < 0) [[unlikely]] begin = 0;

	Cursor::Type typeSave = cursor.type();
	cursor.set (Cursor::Type::CurClear);
	con.clear_lines (begin, end);
	cursor.set (typeSave);
}

}
