#include "Basedline.hpp"

#include <cstdio>

#define BL_MAX_PRINT_LIMIT 64
#define BL_MAX_READ_LIMIT 256

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

void Basedline::do_print () {
	if (readState) // TODO: inputPos.Y may be not in the last input line!
		tty.clear_lines (readState->promptLine, tty.cursor.inputPos.Y);
	size_t printed = 0;
	do {
		tty.puts (printQueue.pop().value(), Cursor::Type::CurOutput);
		printed++;
	}
	while (!printQueue.empty() && printed < BL_MAX_PRINT_LIMIT);
	//if (readState)
	//	tty.restore_lines();
}

void Basedline::print (std::string s) {
	printQueue.push (std::move (s));
}

std::optional<std::string> Basedline::loop () {
	if (!printQueue.empty())
		do_print();
	return std::nullopt;
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
