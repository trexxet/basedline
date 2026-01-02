#pragma once

#include "Defs.hpp"

namespace Basedline {

class ConsoleHandle;

class Cursor {
private:
	ConsoleHandle& con;
public:
	coord_t pos ();
	void move (coord_t pos);

	void input_shift (termsize_t dx);

	Cursor (ConsoleHandle& con) : con (con) { }
	BASEDLINE_CLASS_NO_COPY_MOVE (Cursor);
};

}
