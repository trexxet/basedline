#pragma once

#include <memory>
#include <string>

#include "Debug.hpp"
#include "Defs.hpp"
#include "TTY.hpp"

namespace Basedline {

struct ReadState {
	std::string prompt, linebuf;
	termsize_t promptLine;
};

class Basedline {
	Debug dbg;
	TTY tty;
	void restore_input ();

	std::unique_ptr<ReadState> readState;

	void read_input ();
public:
	void read (const std::string& prompt);
	void print (const std::string& s);

	Basedline ();
	~Basedline ();
	BASEDLINE_CLASS_NO_COPY_MOVE (Basedline);
};

}
