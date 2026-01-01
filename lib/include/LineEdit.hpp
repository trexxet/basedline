#pragma once

#include "ReadState.hpp"
#include "TTY.hpp"

namespace Basedline::LineEdit {

bool is_lineedit (const TTY::Input& input);
void apply (TTY::Input& input, ReadState& rs);
void apply_lr (TTY::Input& input, ReadState& rs);

}
