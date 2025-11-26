#include "basedline.hpp"

#include <cstdio>

#include "debug.hpp"

namespace Basedline {

/// @brief Print line buffer
void Basedline::restore_input () {
	tty.puts (linebuf, Cursor::Type::CurInput);
}

void Basedline::read (std::string_view prompt) {
	promptLine = tty.cursor.prepare_input (prompt.length());
	linebuf = std::string (prompt);
	restore_input();

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
	linebuf.clear();
}

void Basedline::print (const std::string& s) {
	//tty.clear_lines (promptLine, tty.cursor.inputPos.Y);
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
