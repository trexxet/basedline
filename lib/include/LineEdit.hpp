#pragma once

#include "Input.hpp"
#include "ReadState.hpp"

namespace Basedline::LineEdit {

bool is_lineedit (const Input& input);
void apply (Input& input, ReadState& rs);
void apply_lr (Input& input, ReadState& rs);

}
