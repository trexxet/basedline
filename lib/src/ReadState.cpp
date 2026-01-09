#include "ReadState.hpp"

#include <algorithm>

#include "ConsoleHandle.hpp"

namespace Basedline {

void ReadState::redraw_from_cursor () {
	con.puts (std::string_view (linebuf).substr (std::min (linebufCursor, linebufCursorSave)));
	if (deletes > 0) {
		con.clear_chars (deletes);
		deletes = 0;
	}
	con.cursor.move ({ // move to linebuf pos
		static_cast<termsize_t> (prompt.length() + linebufCursor), inputLine
	});
	dirty = false;
}

void ReadState::redraw_with_prompt () {
	con.cursor.move ({0, inputLine});
	con.puts (prompt);
	con.puts (linebuf);
	con.cursor.move ({ // move to linebuf pos
		static_cast<termsize_t> (prompt.length() + linebufCursor), inputLine
	});
	dirty = false;
}

void ReadState::restore_after_print (coord_t& lastPrintPos) {
	inputLine = con.bottom_line();
	if (lastPrintPos.Y >= inputLine && lastPrintPos.X > 0)
		con.resolve_io_line_overlap (inputLine, lastPrintPos.Y);
	redraw_with_prompt();
}

}
