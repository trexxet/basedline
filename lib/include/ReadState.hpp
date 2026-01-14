#pragma once

#include <string>

#include "Defs.hpp"

namespace Basedline {

class ConsoleHandle;

struct ReadState {
	ConsoleHandle& con;
	std::string prompt, linebuf;
	termsize_t inputLine, inputHeight;
	size_t linebufCursor, linebufCursorSave;

	size_t deletes;
	bool dirty;

	void redraw_from_cursor ();
	void redraw_with_prompt ();
	void restore_after_print ();

	ReadState (ConsoleHandle& con, const std::string& prompt, termsize_t inputLine) :
		con (con), prompt (prompt), linebuf (""), inputLine (inputLine),
		inputHeight (1), linebufCursor (0), dirty (false), deletes (0) { }
};

}
