#pragma once

#include <string>

#include "Defs.hpp"

namespace Basedline {

class ConsoleHandle;

struct ReadState {
	ConsoleHandle& con;
	std::string prompt, linebuf;
	termsize_t inputLine, inputHeight;
	size_t linebufCursor;
	bool dirty, insert;

	void redraw_from_cursor ();
	void redraw_with_prompt ();
	void restore_after_print (coord_t& lastPrintPos);

	ReadState (ConsoleHandle& con, const std::string& prompt, termsize_t inputLine) :
		con (con), prompt (prompt), linebuf (""), inputLine (inputLine),
		inputHeight (1), linebufCursor (0), dirty (false), insert (false) { }
};

}
