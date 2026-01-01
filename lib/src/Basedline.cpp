#include "Basedline.hpp"

#include <cctype>

#include "LineEdit.hpp"

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
	if (LineEdit::is_lineedit (input))
		line_edit (input);
	return true;
}

void Basedline::line_edit (TTY::Input& input) {
	LineEdit::apply (input, readState.value());
	if (input.is_left ())
		tty.cursor.input_shift (-1);
	if (input.is_right ())
		tty.cursor.input_shift (1);
}

bool Basedline::read (const std::string& prompt) {
	if (readState) return false;
	readState.emplace (prompt, tty.cursor.inputPos.Y);
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
		else if (readState->dirty) {
			// TODO: separate
			if (readState->linebufCursor == readState->linebuf.length()) {
				tty.putc (readState->linebuf.back(), Cursor::Type::CurInput);
				readState->dirty = false;
			}
			// TODO: redraw from middle
		}
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
