#pragma once

#include <concepts>
#include <type_traits>

#include "Defs.hpp"

namespace Basedline::Console {
	struct BaseHandle;
}

namespace Basedline::Cursor {

enum Type { CurInput, CurOutput, CurClear, count };

class BaseCursor {
protected:
	Console::BaseHandle& con;
	Type currType;
	coord_t pos[Type::count];
public:
	const coord_t& outputPos = pos[Type::CurOutput];
	void save ();
	inline Type type () { return currType; }
	void set (Type type);
	void move (coord_t pos);
	BaseCursor (Console::BaseHandle& tty);
};

using OCursor = BaseCursor;

class IOCursor : public BaseCursor {
	coord_t promptEnd;
public:
	const coord_t& inputPos = pos[Type::CurInput];
	termsize_t prepare_input (size_t promptLength);
	void input_shift (termsize_t dx);
	void input_move_down ();
	IOCursor (Console::BaseHandle& tty) : BaseCursor (tty), promptEnd {0} { }
};

template<typename T>
concept IOClass = std::derived_from<T, BaseCursor>;

}
