#include "InputDispatcher.hpp"

#include <variant>

#include "Basedlib/Meta/Overloaded.hpp"

namespace Basedline {

static void handle_key_input (const BasedVT::InputEvent::KeyInput& k, Editor& ed) {
	using namespace BasedVT::InputEvent;
	switch (k.key) {
		[[likely]]
		case KeyInput::Key::CHAR: ed.accumulate (k.byte); return;
		case KeyInput::Key::LEFT:
			if (!k.ctrl) [[likely]]
				ed.move_prev_grapheme();
			else
				ed.move_prev_word();
			return;
		case KeyInput::Key::RIGHT:
			if (!k.ctrl) [[likely]]
				ed.move_next_grapheme();
			else
				ed.move_next_word();
			return;
		case KeyInput::Key::HOME: ed.move_begin(); return;
		case KeyInput::Key::END: ed.move_end(); return;
	}
}

void dispatch_input_event (const BasedVT::InputEvent::Event& ev, Editor& ed) {
	std::visit (Basedlib::Overloaded {
		[&] (BasedVT::InputEvent::KeyInput k) { handle_key_input (k, ed); }
	}, ev);
}

}
