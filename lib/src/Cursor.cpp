#include "Cursor.hpp"

#include "Debug.hpp"
#include "TTY.hpp"

#if defined(BASEDLINE_DEBUG)
static const char *cursorTypeName[] = {
	"CurInput", "CurOutput", "CurClear"
};
#endif

namespace Basedline {

void Cursor::save () {
#if defined(_WIN32)
	pos[currType] = con.csbi().dwCursorPosition;
#endif
}

void Cursor::set (Type type) {
	if (type == currType) return;
	save();
#if defined(_WIN32)
	BL_DEBUG ("Switch hOut {} cursor type {} -> {}\n", con.hOut,
	          cursorTypeName[currType], cursorTypeName[type]);
#endif
	currType = type;
	move (pos[type]);
}

void Cursor::move (coord_t pos) {
#if defined(_WIN32)
	SetConsoleCursorPosition (con.hOut, pos);
#endif
}

// TODO: multiline wrap
void Cursor::input_shift (termsize_t dx) {
	set (Type::CurInput);
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi = con.csbi();
	termsize_t x = csbi.dwCursorPosition.X + dx;
	move ({x, csbi.dwCursorPosition.Y});
#endif
}

void Cursor::input_move_down () {
	set (Type::CurInput);
#if defined(_WIN32)
	termsize_t inputLineY = con.csbi().srWindow.Bottom;
#endif
	move ({0, inputLineY});
}

Cursor::Cursor (ConsoleHandle& con) : con (con) {
	currType = Type::CurOutput;
	save();
	pos[Type::CurClear] = pos[Type::CurInput] = pos[Type::CurOutput];
}

}
