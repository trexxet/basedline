#include "Basedline.hpp"

#include <cctype>

#define BL_MAX_PRINT_LIMIT 64
#define BL_MAX_READ_LIMIT 256

namespace Basedline {

void Basedline::restore_input () {
	if (!readState) return;
	tty.cursor.input_move_down();
	tty.puts (readState->prompt + readState->linebuf, Cursor::Type::CurInput);
}

// TODO: process input in batches
OptString Basedline::read_input () {
	TTY::Input input;
	size_t read = 0;
	while (tty.has_input() && read < BL_MAX_READ_LIMIT) {
		input = tty.getc();
		if (!process_input(input))
			return std::move (readState->linebuf);
		read++;
	}
	return std::nullopt;
}

bool Basedline::process_input (TTY::Input& input) {
	if (!input.ok()) [[unlikely]] return true; // just ignore input we can't parse now
	if (input.is_eol()) [[unlikely]] return false;
	// handle resize
	// handle autocomplete
	// handle history
	// line edit
	if (std::isprint (input.c)) {
		readState->linebuf += input.c;
		tty.putc (input.c, Cursor::Type::CurInput);
	};
	return true;
}

bool Basedline::read (const std::string& prompt) {
	if (readState) return false;
	readState.emplace (prompt, "", tty.cursor.set_prompt_limit (prompt.length()));
	restore_input();
	return true;
}

void Basedline::do_print () {
	if (readState) // TODO: inputPos.Y may be not in the last input line!
		tty.clear_lines (readState->promptLine, tty.cursor.inputPos.Y);
	size_t printed = 0;
	do {
		tty.puts (printQueue.pop().value(), Cursor::Type::CurOutput);
		printed++;
	} while (!printQueue.empty() && printed < BL_MAX_PRINT_LIMIT);
	if (readState)
		restore_input();
}

void Basedline::print (std::string s) {
	printQueue.push (std::move (s));
}

OptString Basedline::loop () {
	if (!printQueue.empty())
		do_print();
	if (readState) {
		OptString inputBuf = read_input();
		if (inputBuf)
			readState.reset();
		return inputBuf;
	}
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
