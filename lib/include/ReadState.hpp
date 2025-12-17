#pragma once

#include <string>

#include "Defs.hpp"

namespace Basedline {

struct ReadState {
	std::string prompt, linebuf;
	termsize_t promptLine;
};

}
