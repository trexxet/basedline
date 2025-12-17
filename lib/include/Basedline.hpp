#pragma once

#include <optional>
#include <string>

#include "Debug.hpp"
#include "Defs.hpp"
#include "ReadState.hpp"
#include "TTY.hpp"

namespace Basedline {

class Basedline {
	Debug dbg;
	TTY tty;
	void restore_input ();

	std::optional<ReadState> readState;

	void read_input ();
public:
	void read (const std::string& prompt);
	void print (const std::string& s);

	Basedline ();
	~Basedline ();
	BASEDLINE_CLASS_NO_COPY_MOVE (Basedline);
};

}
