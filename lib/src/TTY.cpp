#include "TTY.hpp"

#include <cstdio>
#include <format>

#include "Debug.hpp"

#define CSI "[\x1b"

namespace Basedline {

bool TTY::set_raw (bool raw) {
	return raw ? con.enable_raw() : con.disable_raw();
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
			cursor.input_shift (-1);
			break;
		case VK_RIGHT:
			input.flags[Input::Flags::IS_RIGHT] = true;
			cursor.input_shift (1);
			break;
	}
#endif
}

void TTY::backspace () {
	std::printf (CSI "0M");
}

void TTY::clear_lines (termsize_t begin, termsize_t end) {
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO tcsbi = con.csbi();
	coord_t scrBufSize = tcsbi.dwSize;
#endif

	if (begin > end) [[unlikely]] return;
	if (begin < 0) [[unlikely]] begin = 0;
	if (end >= scrBufSize.Y) [[unlikely]] end = scrBufSize.Y - 1;

	Cursor::Type typeSave = cursor.type();
	cursor.set (Cursor::Type::CurClear);

#if defined(_WIN32)
	coord_t startPos = {0, begin};
	DWORD charsToWrite = (end - begin + 1) * scrBufSize.X;
	DWORD written;
	FillConsoleOutputCharacter (con.hOut, ' ', charsToWrite, startPos, &written);
	FillConsoleOutputAttribute (con.hOut, tcsbi.wAttributes, charsToWrite, startPos, &written);
#endif

	cursor.set (typeSave);
}

TTY::Input TTY::getc () {
	Input input;
#if defined(_WIN32)
	while (true) {
		INPUT_RECORD inputRec;
		DWORD inputCount;
		bool skip = false;

		if (!ReadConsoleInput(con.hIn, &inputRec, 1, &inputCount)
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

		Debug::print (std::format
			("input {} mods 0x{:04x} virt 0x{:04x} chr 0x{:04x} ('{}')\n",
			 input.flags[Input::Flags::HAS_CTRL] ? "CTRL" : "",
			 mods, input.vkey, input.ch, 
			 std::isprint(input.ch) ? input.ch : ' '));

		return input;
	}
#endif
}

void TTY::putc (int c, Cursor::Type curType) {
	cursor.set (curType);
	con.putc (c);
}

void TTY::puts (const std::string& s, Cursor::Type curType) {
	cursor.set (curType);
	if (curType == Cursor::Type::CurOutput) {
		coord_t oldPos = cursor.outputPos;
	}
	con.puts (s);
}

TTY::TTY () {
	vbuf.con.sync_settings (con.csbi());
}

}
