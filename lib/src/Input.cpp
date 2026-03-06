#include "Input.hpp"

#include "Basedlib/Overloaded.hpp"

#include "Console.hpp"
#include "Debug.hpp"

namespace Basedline {

void KeyInputEvent::process_control_key (const RawInputEvent& rawInput) {
#if BLWIN
	flags[KeyInputEvent::Flags::HAS_CTRL] = rawInput.ev.key.mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);
	switch (rawInput.ev.key.vkey) {
		case VK_RETURN:
			flags[KeyInputEvent::Flags::IS_EOL] = true;
			break;
		case VK_LEFT:
			flags[KeyInputEvent::Flags::IS_LEFT] = true;
			break;
		case VK_RIGHT:
			flags[KeyInputEvent::Flags::IS_RIGHT] = true;
			break;
		case VK_UP:
			flags[KeyInputEvent::Flags::IS_UP] = true;
			break;
		case VK_DOWN:
			flags[KeyInputEvent::Flags::IS_DOWN] = true;
			break;
		case VK_BACK:
			flags[KeyInputEvent::Flags::IS_BKSPC] = true;
			break;
		case VK_DELETE:
			flags[KeyInputEvent::Flags::IS_DEL] = true;
			break;
		case ' ':
			flags[KeyInputEvent::Flags::IS_SPACE] = true;
			break;
		case 'D':
		case 'Z':
			if (flags[KeyInputEvent::Flags::HAS_CTRL])
				flags[KeyInputEvent::Flags::IS_EOL] = true;
			break;
		default: break;
	}
#endif
}

KeyInputEvent KeyInputEvent::make_from_raw (const RawInputEvent& rawInput) {
	KeyInputEvent kinput;
	kinput.c = static_cast<int> (rawInput.ev.key.ch);
	kinput.flags[KeyInputEvent::Flags::IS_OK] = true;
	kinput.process_control_key (rawInput);

	BL_DEBUG ("ok {} ctrl {} c {}\n", kinput.ok(), kinput.has_ctrl(), kinput.c);

	return kinput;
}

bool KeyInputEvent::process (Console& con, ReadState& rs) {
	if (!ok()) [[unlikely]] return true; // just ignore input we can't parse now
	if (is_eol()) [[unlikely]] return false;
	if (has_ctrl()) [[unlikely]] {
		if (is_up()) con.out.scroll_up();
		else if (is_down()) con.out.scroll_down();
		else if (is_space()) con.out.toggle_lock();
		return true;
	}
	// TODO: handle autocomplete
	// TODO: handle history
	//if (LineEdit::is_lineedit (*this))
	//	apply_line_edit (con, rs);
	return true;
}

bool ResizeInputEvent::process (Console& con) {
	//con.resizeDebounce.trigger();
	//con.pendingResize = true;
	return true;
}

InputEvent InputEvent::get (Console& con) {
	RawInputEvent rawInput = con.get_raw_input();
	InputVariant var;

	switch (rawInput.type) {
		case RawInputEvent::Type::Key:
			var = KeyInputEvent::make_from_raw (rawInput); break;
		case RawInputEvent::Type::Resize:
			var = ResizeInputEvent (rawInput.ev.newSize); break;
		case RawInputEvent::Type::Unknown: default:
			var = KeyInputEvent::make_err(); break;
	}

	return InputEvent::make (std::move (var));
}

bool InputEvent::process (Console& con, OptReadState& rs) {
	// TODO: value.visit when c++26
	return std::visit (Basedlib::Overloaded {
		[&] (KeyInputEvent& k)    { return rs ? k.process (con, rs.value()) : true; },
		[&] (ResizeInputEvent& r) { return r.process (con); }
	}, value);
}

void InputHandler::sync () {
	werase (pad.ncpad);
	acLine = std::wstring (con.size.x, L' ');

	attron (A_REVERSE);
	mvaddwstr (con.size.y - con.acLineY, 0, acLine.c_str());
	attroff (A_REVERSE);

	mvwaddwstr (pad.ncpad, 0, 0, L"> input");

	pad.draw();
}

InputHandler::InputHandler (Console& con, size_t bufLines) : con (con), pad (con.size, bufLines) {
	pad.area = {con.size.y - con.acLineY + 1, 0, con.size.y - 1, con.size.x - 1};
}

}
