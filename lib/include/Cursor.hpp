#pragma once

#include "Defs.hpp"

namespace Basedline {

class ConsoleBuffer;

class Cursor {
public:
	enum Type { CurInput, CurOutput, CurClear, count };
protected:
	ConsoleBuffer& tty;
	Type currType;
	coord_t pos[Type::count];
public:
	const coord_t& inputPos = pos[Type::CurInput];
	const coord_t& outputPos = pos[Type::CurOutput];
	void save ();
	inline Type type () { return currType; }
	void set (Type type);
	void move (coord_t pos);
	Cursor (ConsoleBuffer& tty);
};

class TTYCursor : public Cursor {
	coord_t promptEnd;
public:
	termsize_t prepare_input (size_t promptLength);
	void input_shift (termsize_t dx);
	void input_move_down ();
	TTYCursor (ConsoleBuffer& tty) : Cursor (tty), promptEnd {0} { }
};

}
