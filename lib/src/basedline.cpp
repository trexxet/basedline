#include "basedline.hpp"

#include <cstdio>

#include "debug.hpp"

namespace Basedline {

/// @brief Print line buffer
void Basedline::pbuf () {
	std::printf (linebuf.c_str());
}

void Basedline::read (std::string_view prompt) {
	tty.cursor.input_move_down();
	linebuf = std::string (prompt);
	pbuf();

	tty.set_prompt (std::string (prompt));

	TTY::Input input;
	int c;
	while (true) {
		input = tty.getc();
		if (!input.ok() || input.is_eof()) break;
		c = input.c();
		if (std::isprint (c)) {
			linebuf += c;
			tty.putc (c, Cursor::Type::CurInput);
		};
	}
}

void Basedline::print (const std::string& s) {
	tty.puts (s, Cursor::Type::CurOutput);
}

Basedline::Basedline () {
	Debug::open();
	if (!tty.set_raw (true))
		std::fprintf (stderr, "Failed to enable tty raw mode");
}

Basedline::~Basedline () {
	Debug::close();
	if (!tty.set_raw (false))
		std::fprintf (stderr, "Failed to disable tty raw mode");
}

}
