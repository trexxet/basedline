#pragma once

#include "Input.hpp"
#include "ReadState.hpp"

namespace Basedline::LineEdit {

bool is_lineedit (const KeyInput& input);
void apply (KeyInput& input, ReadState& rs);

}
