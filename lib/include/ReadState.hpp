#pragma once

#include <string>

#include "Defs.hpp"

namespace Basedline {

struct ReadState {
	std::string prompt;
	std::string linebuf = "";
	termsize_t inputHeight = 1;
	size_t linebufCursor = 0, linebufCursorSave;

	size_t deletes = 0;
	bool dirty = false;

	inline size_t len () const { return prompt.length() + linebuf.length(); }
	inline size_t lenToCursor () const { return prompt.length() + linebufCursor; }

	ReadState (const std::string& prompt) : prompt (prompt) { }
};

}
