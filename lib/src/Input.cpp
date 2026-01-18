#include "Input.hpp"

#include "ConsoleHandle.hpp"
#include "Debug.hpp"

namespace Basedline {

void KeyInput::process_control_key (const RawInput& rawInput) {
#if defined(_WIN32)
	flags[KeyInput::Flags::HAS_CTRL] = rawInput.ev.key.mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);
	switch (rawInput.ev.key.vkey) {
		case VK_RETURN:
			flags[KeyInput::Flags::IS_EOL] = true;
			break;
		case VK_LEFT:
			flags[KeyInput::Flags::IS_LEFT] = true;
			break;
		case VK_RIGHT:
			flags[KeyInput::Flags::IS_RIGHT] = true;
			break;
		case VK_BACK:
			flags[KeyInput::Flags::IS_BKSPC] = true;
			break;
		case VK_DELETE:
			flags[KeyInput::Flags::IS_DEL] = true;
			break;
		case 'D':
		case 'Z':
			if (flags[KeyInput::Flags::HAS_CTRL])
				flags[KeyInput::Flags::IS_EOL] = true;
			break;
		default: break;
	}
#endif
}

KeyInput KeyInput::make_from_raw (const RawInput& rawInput) {
	KeyInput kinput;
	kinput.c = static_cast<int> (rawInput.ev.key.ch);
	kinput.flags[KeyInput::Flags::OK] = true;
	kinput.process_control_key (rawInput);

#if defined(_WIN32) && 0
	BL_DEBUG ("input {} mods 0x{:04x} virt 0x{:04x} chr 0x{:04x} ('{}')\n",
				kinput.flags[KeyInput::Flags::HAS_CTRL] ? "CTRL" : "",
				rawInput.mods, rawInput.vkey, kinput.c, 
				kinput.is_print() ? (char) kinput.c : ' ');
#endif

	return kinput;
}

Input Input::make (InputVariant&& var) {
	return Input (std::move (var));
}

Input Input::get (ConsoleHandle& con) {
	RawInput rawInput = con.get_raw_input();
	InputVariant var;

	switch (rawInput.type) {
		case RawInput::Type::Key:
			var = KeyInput::make_from_raw (rawInput); break;
		case RawInput::Type::Resize:
			var = KeyInput::make_err(); break;
		case RawInput::Type::Unknown: default:
			var = KeyInput::make_err(); break;
	}

	return Input::make (std::move (var));
}

}
