#include "ReadState.hpp"

#include "ConsoleHandle.hpp"

namespace Basedline {

void ReadState::redraw_from_cursor () {
	if (linebufCursor == linebuf.length() && insert) {
		con.putc (linebuf.back());
	} else {
		con.puts (linebuf.substr (insert ? linebufCursor - 1 : linebufCursor));
		if (!insert) con.clear_char();
		con.cursor.move ({ // move to linebuf pos
			static_cast<termsize_t> (prompt.length() + linebufCursor), inputLine
		});
	}
	dirty = insert = false;
}

void ReadState::redraw_with_prompt () {
	con.cursor.move ({0, inputLine});
	con.puts (prompt + linebuf);
	con.cursor.move ({ // move to linebuf pos
		static_cast<termsize_t> (prompt.length() + linebufCursor), inputLine
	});
	dirty = insert = false;
}

void ReadState::restore_after_print (coord_t& lastPrintPos) {
	inputLine = con.bottom_line();
	if (lastPrintPos.Y >= inputLine && lastPrintPos.X > 0)
		con.resolve_io_line_overlap (inputLine, lastPrintPos.Y);
	redraw_with_prompt();
}

}
