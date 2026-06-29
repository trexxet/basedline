#pragma once

#include "Basedline/Defs.hpp"

namespace Basedline::VtCommand {

void clear ();
void clear_lines (tsize_t from, tsize_t count);
void cursor_to (tpos_t pos);

}
