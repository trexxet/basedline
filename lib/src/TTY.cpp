#include "TTY.hpp"

#include <cctype>

#include "Debug.hpp"

namespace Basedline {

bool TTY::set_raw (bool raw) {
	return raw ? con.enable_raw() : con.disable_raw();
}

void TTY::process_control_key (TTY::Input& input, const ConsoleHandle::RawInput& rawInput) {
#if defined(_WIN32)
	input.flags[Input::Flags::HAS_CTRL] = rawInput.mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);
	switch (rawInput.vkey) {
		case VK_RETURN:
			input.flags[Input::Flags::IS_EOL] = true;
			break;
		case VK_LEFT:
			input.flags[Input::Flags::IS_LEFT] = true;
			break;
		case VK_RIGHT:
			input.flags[Input::Flags::IS_RIGHT] = true;
			break;
		case 'D':
		case 'Z':
			if (input.flags[Input::Flags::HAS_CTRL])
				input.flags[Input::Flags::IS_EOL] = true;
			break;
		default: break;
	}
#endif
}

TTY::Input TTY::getc () {
	Input input;
	ConsoleHandle::RawInput rawInput = con.get_input();
	if (rawInput.type == ConsoleHandle::RawInput::Type::Unknown)
		return Input::make_err();

	input.c = static_cast<int> (rawInput.ch);
	input.flags[Input::Flags::OK] = true;
	process_control_key (input, rawInput);

#if defined(_WIN32)
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

void TTY::putc (int c) {
	con.putc (c);
}

void TTY::puts (const std::string& s) {
	con.puts (s);
}

void TTY::clear_lines (termsize_t begin, termsize_t end) {
	if (begin > end) [[unlikely]] return;
	if (begin < 0) [[unlikely]] begin = 0;
	con.clear_lines (begin, end);
}

termsize_t TTY::bottom_line () {
	return con.bottom_line();
}

}
