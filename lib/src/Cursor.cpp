#include "Cursor.hpp"

#include "TTY.hpp"

namespace Basedline {

void Cursor::BaseCursor::save () {
#if defined(_WIN32)
	pos[currType] = con.csbi().dwCursorPosition;
#endif
}

void Cursor::BaseCursor::set (Cursor::Type type) {
	if (type == currType) return;
	save();
	currType = type;
	move (pos[type]);
}

void Cursor::BaseCursor::move (coord_t pos) {
#if defined(_WIN32)
	SetConsoleCursorPosition (con.hOut, pos);
#endif
}

Cursor::BaseCursor::BaseCursor (ConsoleHandle& con) : con (con) {
	currType = Type::CurOutput;
	save();
	pos[Type::CurClear] = pos[Type::CurInput] = pos[Type::CurOutput];
}

termsize_t Cursor::TTYCursor::prepare_input (size_t promptLength) {
	input_move_down();
	promptEnd = {static_cast<termsize_t>(promptLength), pos[Type::CurInput].Y};
	return promptEnd.Y;
}

void Cursor::TTYCursor::input_shift (termsize_t dx) {
	set (Type::CurInput);
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi = con.csbi();
	termsize_t x = csbi.dwCursorPosition.X + dx;
	if (x < promptEnd.X) return;
	move ({x, csbi.dwCursorPosition.Y});
#endif
}

void Cursor::TTYCursor::input_move_down () {
	set (Type::CurInput);
#if defined(_WIN32)
	termsize_t inputLineY = con.csbi().srWindow.Bottom;
#endif
	move ({0, inputLineY});
}

}
