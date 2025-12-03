#include "Cursor.hpp"

#include "TTY.hpp"

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

termsize_t TTYCursor::prepare_input (size_t promptLength) {
	input_move_down();
	promptEnd = {static_cast<termsize_t>(promptLength), pos[Type::CurInput].Y};
	return promptEnd.Y;
}

void TTYCursor::input_shift (termsize_t dx) {
	set (Type::CurInput);
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi = tty.csbi();
	termsize_t x = csbi.dwCursorPosition.X + dx;
	if (x < promptEnd.X) return;
	move ({x, csbi.dwCursorPosition.Y});
#endif
}

void TTYCursor::input_move_down () {
	set (Type::CurInput);
#if defined(_WIN32)
	termsize_t inputLineY = tty.csbi().srWindow.Bottom;
#endif
	move ({0, inputLineY});
}

}
