#include "Basedline.hpp"

#include <cstdio>
#include <format>

#include "Debug.hpp"

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
	coord_t printEnd = tty.puts_vbuf (s);
	Debug::print (std::format("printEnd [{} {}] promptLine {}\n", printEnd.X, printEnd.Y, promptLine));
	if (printEnd.Y >= promptLine && printEnd.X > 0) {
		Debug::print ("printEnd intersects input");
		//tty.clear_lines (promptLine, tty.cursor.inputPos.Y);
	}
	tty.puts (s, Cursor::Type::CurOutput);
}

Basedline::Basedline () {
	if (!tty.set_raw (true))
		std::fprintf (stderr, "Failed to enable tty raw mode");
}

Basedline::~Basedline () {
	if (!tty.set_raw (false))
		std::fprintf (stderr, "Failed to disable tty raw mode");
}

}
