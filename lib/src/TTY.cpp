#include "TTY.hpp"

#include <cctype>

#include "Debug.hpp"

#define CSI "[\x1b"

namespace Basedline {

bool TTY::set_raw (bool raw) {
	return raw ? con.enable_raw() : con.disable_raw();
}

void TTY::ctrl (TTY::Input& input, const ConsoleHandle::RawInput& rawInput) {
	switch (rawInput.vkey) {
		// Handle Ctrl-D and Ctrl-Z
		case 'D':
		case 'Z':
			input.flags[Input::Flags::IS_EOL] = true;
			break;
		default: break;
	}
}

void TTY::left_right (TTY::Input& input, const ConsoleHandle::RawInput& rawInput) {
	switch (rawInput.vkey) {
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
	ConsoleHandle::RawInput rawInput = con.get_input();
	if (rawInput.type == ConsoleHandle::RawInput::Type::Unknown)
		return Input::make_err();

	input.c = static_cast<int> (rawInput.ch);
	input.flags[Input::Flags::OK] = true;

#if defined(_WIN32)
	// Ctrl
	input.flags[Input::Flags::HAS_CTRL] = rawInput.mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);
	if (input.flags[Input::Flags::HAS_CTRL]) {
		ctrl (input, rawInput);
		return input;
	}

	// Arrows
	if (rawInput.vkey == VK_LEFT || rawInput.vkey == VK_RIGHT)
		left_right (input, rawInput);

	// Enter
	if (rawInput.vkey == VK_RETURN)
		input.flags[Input::Flags::IS_EOL] = true;

	BL_DEBUG ("input {} mods 0x{:04x} virt 0x{:04x} chr 0x{:04x} ('{}')\n",
				input.flags[Input::Flags::HAS_CTRL] ? "CTRL" : "",
				rawInput.mods, rawInput.vkey, input.c, 
				std::isprint (input.c) ? input.c : ' ');
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
