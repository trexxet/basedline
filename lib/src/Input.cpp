#include "Input.hpp"

#include <cctype>

#include "ConsoleHandle.hpp"
#include "Debug.hpp"

namespace Basedline {

void Input::process_control_key (const RawInput& rawInput) {
#if defined(_WIN32)
	flags[Input::Flags::HAS_CTRL] = rawInput.mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);
	switch (rawInput.vkey) {
		case VK_RETURN:
			flags[Input::Flags::IS_EOL] = true;
			break;
		case VK_LEFT:
			flags[Input::Flags::IS_LEFT] = true;
			break;
		case VK_RIGHT:
			flags[Input::Flags::IS_RIGHT] = true;
			break;
		case 'D':
		case 'Z':
			if (flags[Input::Flags::HAS_CTRL])
				flags[Input::Flags::IS_EOL] = true;
			break;
		default: break;
	}
#endif
}

Input Input::make_from_raw (const RawInput& rawInput) {
	Input input;
	input.c = static_cast<int> (rawInput.ch);
	input.flags[Input::Flags::OK] = true;
	input.process_control_key (rawInput);
	return input;
}

Input Input::get (ConsoleHandle& con) {
	RawInput rawInput = con.get_raw_input();
	if (rawInput.type == RawInput::Type::Unknown)
		return Input::make_err();
	Input input = Input::make_from_raw (rawInput);

#if defined(_WIN32)
	BL_DEBUG ("input {} mods 0x{:04x} virt 0x{:04x} chr 0x{:04x} ('{}')\n",
				input.flags[Input::Flags::HAS_CTRL] ? "CTRL" : "",
				rawInput.mods, rawInput.vkey, input.c, 
				std::isprint (input.c) ? input.c : ' ');
#endif

	return input;
}

}
