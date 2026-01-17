#include "Output.hpp"

#include <algorithm>

#if defined(_WIN32)
# define IF_CONPTY if (con.is_conpty())
#else
# define IF_CONPTY if (true)
#endif

namespace Basedline {

void Output::print (std::string_view s) {
	con.puts (s);
	printPos = con.cursor.pos();
	if (con.is_last_column (printPos.X)) {
		con.putc ('\n');
		printPos = con.cursor.pos();
	}
}

void Output::redraw_rs_from_cursor (ReadState& rs) {
	termsize_t newHeight = con.cursor.wrap (inputLine, rs.len()).Y - inputLine + 1;
	if (newHeight > rs.inputHeight) [[unlikely]] {
		termsize_t linesToScroll = newHeight - rs.inputHeight;
		con.scroll (linesToScroll);
		IF_CONPTY {
			inputLine -= linesToScroll;
			printPos.Y -= linesToScroll;
		}
	}
	rs.inputHeight = newHeight;

	con.puts (std::string_view (rs.linebuf).substr (std::min (rs.linebufCursor, rs.linebufCursorSave)));

	if (rs.deletes > 0) {
		con.clear_chars (rs.deletes);
		rs.deletes = 0;
	}

	con.cursor.move (con.cursor.wrap (inputLine, rs.lenToCursor()));
	rs.dirty = false;
}

void Output::redraw_rs_with_prompt (ReadState& rs) {
	con.cursor.move ({0, inputLine});
	con.puts (rs.prompt);
	con.puts (rs.linebuf);
	inputLine = con.bottom_line() - rs.inputHeight + 1;

	IF_CONPTY {
		if (rs.len() % con.line_width() == 0 && rs.len() > 0)
			con.putc ('\n');
	}

	con.cursor.move (con.cursor.wrap (inputLine, rs.lenToCursor()));
	rs.dirty = false;
}

void Output::restore_rs_after_print (ReadState& rs) {
	inputLine = std::max (con.bottom_line() - rs.inputHeight + 1, printPos.X > 0 ? printPos.Y + 1 : printPos.Y);
	IF_CONPTY {
		printPos.Y -= (rs.inputHeight - 1) - (con.bottom_line() - inputLine);
		if (inputLine > con.bottom_line()) {
			con.scroll (1);
			inputLine--;
		}
	}

	redraw_rs_with_prompt (rs);
}

Output::Output (ConsoleHandle& con) : con (con) {
	printPos = con.cursor.pos();
	inputLine = con.bottom_line();
}

}
