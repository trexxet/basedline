#include "cursor.hpp"

#include "tty.hpp"

namespace Basedline {

void Cursor::save () {
#if defined(_WIN32)
	pos[currType] = tty.csbi().dwCursorPosition;
#endif
}

void Cursor::set (Cursor::Type type) {
	if (type == currType) return;
	save();
	currType = type;
	move (pos[type]);
}

void Cursor::move (coord_t pos) {
#if defined(_WIN32)
	SetConsoleCursorPosition (tty.hOut, pos);
#endif
}

Cursor::Cursor (ConsoleBuffer& tty) : tty (tty) {
	currType = Type::CurOutput;
	save();
	pos[Type::CurClear] = pos[Type::CurInput] = pos[Type::CurOutput];
}

}
