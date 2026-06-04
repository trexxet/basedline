#pragma once

#include "BasedVT/InputEvent/Event.hpp"

#include "Basedline/Editor.hpp"

namespace Basedline {

void dispatch_input_event (const BasedVT::InputEvent::Event& ev, Editor& ed);

}
