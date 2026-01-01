#pragma once

#include "Defs.hpp"

namespace Basedline {

class ConsoleHandle;

class Cursor {
public:
	enum Type { CurInput, CurOutput, CurClear, count };
private:
	ConsoleHandle& con;
	Type currType;
	coord_t pos[Type::count];
public:
	const coord_t& inputPos = pos[Type::CurInput];
	const coord_t& outputPos = pos[Type::CurOutput];

	void save ();
	inline Type type () { return currType; }
	void set (Type type);
	void move (coord_t pos);

	void input_shift (termsize_t dx);
	void input_move_down ();

	Cursor (ConsoleHandle& tty);
	BASEDLINE_CLASS_NO_COPY_MOVE (Cursor);
};

}
