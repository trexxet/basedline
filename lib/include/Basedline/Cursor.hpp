#pragma once

#include "Defs.hpp"

namespace Basedline {

class Cursor {
	tpos_t _pos;
public:
	tpos_t pos () const noexcept { return _pos; }
	tpos_t move_to (tpos_t newPos);
};

}
