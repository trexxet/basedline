#include "ReadState.hpp"

#include <algorithm>

#include "ConsoleHandle.hpp"

namespace Basedline {

void ReadState::redraw_from_cursor () {
	con.scroll_to_fit_text (inputLine, inputHeight, prompt.length() + linebuf.length());
	con.puts (std::string_view (linebuf).substr (std::min (linebufCursor, linebufCursorSave)));
	if (deletes > 0) {
		con.clear_chars (deletes);
		deletes = 0;
	}
	con.cursor.move (con.cursor.wrap (inputLine, prompt.length() + linebufCursor));
	dirty = false;
}

void ReadState::redraw_with_prompt () {
	con.cursor.move ({0, inputLine});
	con.puts (prompt);
	con.puts (linebuf);
	inputLine = con.bottom_line() - inputHeight + 1;
	con.cursor.move (con.cursor.wrap (inputLine, prompt.length() + linebufCursor));
	dirty = false;
}

void ReadState::restore_after_print () {
	con.adjust_io_lines (inputLine, inputHeight);
	redraw_with_prompt();
}

}
