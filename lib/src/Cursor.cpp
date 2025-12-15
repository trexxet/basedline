#include "Cursor.hpp"

#include "TTY.hpp"

#if defined(BASEDLINE_DEBUG)
# include <format>
# include "Debug.hpp"
static const char *cursorTypeName[] = {
	"CurInput", "CurOutput", "CurClear"
};
#endif

namespace Basedline::Cursor {

void BaseCursor::save () {
#if defined(_WIN32)
# if defined(BASEDLINE_DEBUG)
	coord_t newPos = con.csbi().dwCursorPosition;
	Debug::print (std::format ("Save hOut {} cursor [{} {}] -> [{} {}]\n",
		con.hOut, pos[currType].X, pos[currType].Y, newPos.X, newPos.Y));
# endif
	pos[currType] = con.csbi().dwCursorPosition;
#endif
}

void BaseCursor::set (Cursor::Type type) {
	if (type == currType) return;
	save();
#if defined(BASEDLINE_DEBUG) && defined(_WIN32)
	Debug::print (std::format ("Switch hOut {} cursor type {} -> {}\n",
		con.hOut, cursorTypeName[currType], cursorTypeName[type]));
#endif
	currType = type;
	move (pos[type]);
}

void BaseCursor::move (coord_t pos) {
#if defined(_WIN32)
	COORD bufsize = con.csbi().dwSize;
	Debug::print (std::format ("Move hOut {} size [{} {}] cursor [{} {}] -> [{} {}]\n",
		con.hOut, bufsize.X, bufsize.Y, this->pos[currType].X, this->pos[currType].Y, pos.X, pos.Y));
	bool ret = SetConsoleCursorPosition (con.hOut, pos);
	Debug::print (std::format ("Move hOut {} cursor ret {}\n", con.hOut, ret));
#endif
}

BaseCursor::BaseCursor (Console::BaseHandle& con) : con (con) {
	currType = Type::CurOutput;
	save();
	pos[Type::CurClear] = pos[Type::CurInput] = pos[Type::CurOutput];
}

termsize_t IOCursor::prepare_input (size_t promptLength) {
	input_move_down();
	promptEnd = {static_cast<termsize_t>(promptLength), pos[Type::CurInput].Y};
	return promptEnd.Y;
}

void IOCursor::input_shift (termsize_t dx) {
	set (Type::CurInput);
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi = con.csbi();
	termsize_t x = csbi.dwCursorPosition.X + dx;
	if (x < promptEnd.X) return;
	move ({x, csbi.dwCursorPosition.Y});
#endif
}

void IOCursor::input_move_down () {
	set (Type::CurInput);
#if defined(_WIN32)
	termsize_t inputLineY = con.csbi().srWindow.Bottom;
#endif
	move ({0, inputLineY});
}

}
