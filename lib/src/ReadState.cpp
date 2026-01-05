#include "ReadState.hpp"

#include "ConsoleHandle.hpp"

namespace Basedline {

void ReadState::redraw (bool withPrompt) {
	// TODO: line wrap
	if (withPrompt) {
		con.cursor.move ({0, inputLine});
		con.puts (prompt + linebuf);
		con.cursor.move ({ // move to linebuf pos
			static_cast<termsize_t> (prompt.length() + linebufCursor), inputLine
		});
	} else { // This branch is called only when the cursor is already at desired insert position
		if (linebufCursor == linebuf.length()) {
			con.putc (linebuf.back());
		} else {
			con.puts (linebuf.substr (linebufCursor - 1));
			con.cursor.move ({ // move to linebuf pos
				static_cast<termsize_t> (prompt.length() + linebufCursor), inputLine
			});
		}
	}
	dirty = false;
}

void ReadState::restore_after_print (coord_t& lastPrintPos) {
	inputLine = con.bottom_line();
	if (lastPrintPos.Y >= inputLine && lastPrintPos.X > 0)
		con.resolve_io_line_overlap (inputLine, lastPrintPos.Y);
	redraw (true);
}


}
