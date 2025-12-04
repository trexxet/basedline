#pragma once

#include <string>
#include <string_view>

#include "Debug.hpp"
#include "Defs.hpp"
#include "TTY.hpp"

namespace Basedline {

class Basedline {
	Debug dbg;
	TTY tty;
	std::string linebuf;
	void restore_input ();
	termsize_t promptLine;
public:
	void read (std::string_view prompt);
	void print (const std::string& s);

	Basedline ();
	~Basedline ();
	BASEDLINE_CLASS_NO_COPY_MOVE (Basedline);
};

}
