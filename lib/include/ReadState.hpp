#pragma once

#include <string>

#include "Defs.hpp"

namespace Basedline {

struct ReadState {
	std::string prompt, linebuf;
	termsize_t promptLine, inputHeight;
	size_t linebufCursor;
	bool dirty;
	
	ReadState (const std::string& prompt, termsize_t promptLine) :
		prompt (prompt), linebuf (""), promptLine (promptLine),
		inputHeight (1), linebufCursor (0), dirty (false) { }
};

}
