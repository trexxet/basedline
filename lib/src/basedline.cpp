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
		c = tty.getc();
		if (c == EOF) break;
		if (std::isprint (c)) {
			linebuf += c;
			tty.putc (c);
		} else
		if (std::iscntrl (c)) {
			tty.cntrl (c);
		}
	}
}

Basedline::Basedline () {
	if (!tty.setRaw (true))
		std::fprintf (stderr, "Failed to enable tty raw mode");
}

Basedline::~Basedline () {
	if (!tty.setRaw (false))
		std::fprintf (stderr, "Failed to disable tty raw mode");
}

}
