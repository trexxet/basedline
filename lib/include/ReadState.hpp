#pragma once

#include <string>

#include "Defs.hpp"

namespace Basedline {

class ConsoleHandle;

struct ReadState {
	ConsoleHandle& con;
	std::string prompt;
	std::string linebuf = "";
	termsize_t inputHeight = 1;
	size_t linebufCursor = 0, linebufCursorSave;

	size_t deletes = 0;
	bool dirty = false;

	void redraw_from_cursor ();
	void redraw_with_prompt ();
	void restore_after_print ();

	ReadState (ConsoleHandle& con, const std::string& prompt)
		: con (con), prompt (prompt) { }
};

}
