#pragma once

#include <string>
#include <string_view>

#include "tty.hpp"

namespace Basedline {

class Basedline {
	std::string linebuf;
	void pbuf ();
public:
	TTY tty;

	void read (std::string_view prompt);

	Basedline ();
	~Basedline ();
};

}
