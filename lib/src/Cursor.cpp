#include "Cursor.hpp"

#include <format>
#include <stdexcept>

#include "ConsoleHandle.hpp"

namespace Basedline {

coord_t Cursor::pos () {
#if defined(_WIN32)
	return con.csbi().dwCursorPosition;
#endif
}

void Cursor::move (coord_t pos) {
#if defined(_WIN32)
	if (!SetConsoleCursorPosition (con.hOut, pos)) [[unlikely]]
		throw std::runtime_error (std::format ("Can't move cursor for hOut {} to [{} {}]", con.hOut, pos.X, pos.Y));
#endif
}

// TODO: multiline wrap
void Cursor::input_shift (termsize_t dx) {
#if defined(_WIN32)
	coord_t curr_pos = pos();
	termsize_t x = curr_pos.X + dx;
	move ({x, curr_pos.Y});
#endif
}

}
