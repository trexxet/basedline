#include "Basedline.hpp"

#include <cctype>

#include "LineEdit.hpp"
#include "Debug.hpp"

#define BL_MAX_PRINT_LIMIT 64
#define BL_MAX_READ_LIMIT 256

namespace Basedline {

void Basedline::print_input () {
	if (!readState) [[unlikely]] return;
	// TODO: line wrap
	tty.con.cursor.move ({0, readState->inputLine});
	tty.puts (readState->prompt + readState->linebuf);
	tty.con.cursor.move ({
		static_cast<termsize_t> (readState->prompt.length() + readState->linebufCursor), readState->inputLine
	});
}

void Basedline::restore_input () {
	if (!readState) [[unlikely]] return;
	readState->inputLine = tty.bottom_line();
	if (outputPos.Y == readState->inputLine && outputPos.X > 0) {
		tty.con.cursor.new_input_line (&readState->inputLine);
		outputPos.Y--;
	}
	print_input ();
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
		tty.con.cursor.shift (-1);
	if (input.is_right ())
		tty.con.cursor.shift (1);
}

bool Basedline::read (const std::string& prompt) {
	if (readState) return false;
	readState.emplace (prompt, tty.bottom_line());
	print_input();
	return true;
}

void Basedline::do_print () {
	if (readState) // TODO: line wrap
		tty.clear_lines (readState->inputLine, readState->inputHeight);
	tty.con.cursor.move (outputPos);
	size_t printed = 0;
	do {
		tty.puts (printQueue.pop().value());
		printed++;
		outputPos = tty.con.cursor.pos();
		if (tty.con.is_last_column (outputPos.X)) {
			tty.puts ("\n");
			outputPos = tty.con.cursor.pos();
		}
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
				tty.putc (readState->linebuf.back());
				readState->dirty = false;
			}
			// TODO: redraw from middle
		}
		return inputBuf;
	}
	return std::nullopt;
}

Basedline::Basedline () {
	if (!tty.con.configure())
		std::fprintf (stderr, "Failed to configure terminal");
	outputPos = tty.con.cursor.pos();
}

Basedline::~Basedline () {
	if (!tty.con.unconfigure())
		std::fprintf (stderr, "Failed to restore terminal configuration");
}

}
