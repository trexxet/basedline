#include "Basedline.hpp"

#include <cctype>

#include "LineEdit.hpp"
#include "Debug.hpp"

#define BL_MAX_PRINT_LIMIT 64
#define BL_MAX_READ_LIMIT 256

namespace Basedline {

// TODO: process input in batches
OptString Basedline::read_input () {
	size_t read = 0;
	while (con.has_input() && read < BL_MAX_READ_LIMIT) {
		Input input = Input::get (con);
		if (!process_input (input))
			return std::move (readState->linebuf);
		read++;
	}
	return std::nullopt;
}

bool Basedline::process_input (Input& input) {
	if (!input.ok()) [[unlikely]] return true; // just ignore input we can't parse now
	if (input.is_eol()) [[unlikely]] return false;
	// handle resize
	// handle autocomplete
	// handle history
	if (LineEdit::is_lineedit (input))
		line_edit (input);
	return true;
}

void Basedline::line_edit (Input& input) {
	LineEdit::apply (input, readState.value());
	if (input.is_left ())
		con.cursor.shift (-1);
	if (input.is_right ())
		con.cursor.shift (1);
}

bool Basedline::read (const std::string& prompt) {
	if (readState) return false;
	readState.emplace (con, prompt, con.bottom_line());
	readState->redraw (true);
	return true;
}

void Basedline::do_print () {
	if (readState) // TODO: line wrap
		con.clear_lines (readState->inputLine, readState->inputHeight);
	con.cursor.move (printPos);
	size_t printed = 0;
	do {
		con.puts (printQueue.pop().value());
		printed++;
		printPos = con.cursor.pos();
		if (con.is_last_column (printPos.X)) {
			con.puts ("\n");
			printPos = con.cursor.pos();
		}
	} while (!printQueue.empty() && printed < BL_MAX_PRINT_LIMIT);
	if (readState)
		readState->restore_after_print (printPos);
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
		else if (readState->dirty)
			readState->redraw (false);
		return inputBuf;
	}
	return std::nullopt;
}

Basedline::Basedline () {
	if (!con.configure())
		std::fprintf (stderr, "Failed to configure terminal");
	printPos = con.cursor.pos();
}

Basedline::~Basedline () {
	if (!con.unconfigure())
		std::fprintf (stderr, "Failed to restore terminal configuration");
}

}
