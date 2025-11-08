#include "basedline.hpp"

#include <cstdio>

namespace Basedline {

/// @brief Print line buffer
void Basedline::pbuf () {
	std::printf (linebuf.c_str());
}

void Basedline::read (std::string_view prompt) {
	linebuf = std::string (prompt);
	pbuf();

	int c;
	while (true) {
		c = std::fgetc (stdin);
		if (c == EOF) break;
		std::fputc (c, stdout);
	}
}

Basedline::Basedline () {
	tty.setRaw (true);
}

Basedline::~Basedline () {
	tty.setRaw (false);
}

}
