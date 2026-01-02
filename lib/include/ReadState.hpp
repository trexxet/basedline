#pragma once

#include <string>

#include "Defs.hpp"

namespace Basedline {

struct ReadState {
	std::string prompt, linebuf;
	termsize_t inputLine, inputHeight;
	size_t linebufCursor;
	bool dirty;
	
	ReadState (const std::string& prompt, termsize_t inputLine) :
		prompt (prompt), linebuf (""), inputLine (inputLine),
		inputHeight (1), linebufCursor (0), dirty (false) { }
};

}
