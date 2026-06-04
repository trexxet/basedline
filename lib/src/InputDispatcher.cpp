#include "InputDispatcher.hpp"

#include <variant>

#include "Basedlib/Meta/Overloaded.hpp"

namespace Basedline {

void dispatch_input_event (const BasedVT::InputEvent::Event& ev, Editor& ed) {
	std::visit (Basedlib::Overloaded {
		[&] (BasedVT::InputEvent::KeyInput k) { ed.accumulate (k.byte); }
	}, ev);
}

}
