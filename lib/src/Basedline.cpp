#include "Basedline.hpp"

#include <cstdio>

#include "Debug.hpp"

namespace Basedline {

/// @brief Print line buffer
void Basedline::restore_input () {
	if (readState)
		tty.puts (readState->linebuf, Cursor::Type::CurInput);
}

void Basedline::read_input () {
	if (!readState) return;
	TTY::Input input;
	int c;
	while (true) {
		input = tty.getc();
		if (!input.ok() || input.is_eof()) break;
		c = input.c();
		if (std::isprint (c)) {
			readState->linebuf += c;
			tty.putc (c, Cursor::Type::CurInput);
		};
	}
}

void Basedline::read (const std::string& prompt) {
	if (readState) return;

	readState.emplace (prompt, prompt, tty.cursor.prepare_input (prompt.length()));
	restore_input();

	read_input();
	readState.reset();
}

void Basedline::print (const std::string& s) {
	if (readState)
		tty.clear_lines (readState->promptLine, tty.cursor.inputPos.Y);
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
