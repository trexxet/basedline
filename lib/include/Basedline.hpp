#pragma once

#include <string>
#include <string_view>

#include "TTY.hpp"

namespace Basedline {

class Basedline {
	TTY tty;
	std::string linebuf;
	void restore_input ();
	termsize_t promptLine;
public:
	void read (std::string_view prompt);
	void print (const std::string& s);

	Basedline ();
	~Basedline ();
};

}
