#include "basedline.hpp"

#include <cstdio>

namespace Basedline {

/// @brief Print line buffer
void Basedline::pbuf () {
	std::printf (linebuf.c_str());
}

void Basedline::read (std::string_view prompt) {
	linebuf = std::string (prompt);
	tty.prompt = prompt;
	pbuf();

	TTY::Input input;
	int c;
	while (true) {
		input = tty.getc();
		if (!input.ok() || input.is_eof()) break;
		c = input.c();
		if (std::isprint (c)) {
			linebuf += c;
			tty.putc (c);
		};
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
