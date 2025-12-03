#pragma once

#include "defs.hpp"

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

}
