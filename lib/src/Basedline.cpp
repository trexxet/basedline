#include "Basedline.hpp"

#include "Debug.hpp"

#define BL_MAX_PRINT_LIMIT 64
#define BL_MAX_READ_LIMIT 256

namespace Basedline {

// TODO: process input in batches
OptString Basedline::read_input () {
	size_t read = 0;
	if (readState)
		readState->linebufCursorSave = readState->linebufCursor;
	while (con.has_input() && read < BL_MAX_READ_LIMIT) {
		Input input = Input::get (con);
		if (!input.process (con, readState) && readState)
			return std::move (readState->linebuf);
		read++;
	}
	return std::nullopt;
}

bool Basedline::read (const std::string& prompt) {
	if (readState) return false;
	readState.emplace (prompt);
	out.redraw_rs_with_prompt (readState.value());
	return true;
}

void Basedline::do_print () {
	if (readState)
		con.clear_lines (out.inputLine, readState->inputHeight);
	con.cursor.move (out.printPos);
	size_t printed = 0;
	do {
		out.print (printQueue.pop().value());
		printed++;
	} while (!printQueue.empty() && printed < BL_MAX_PRINT_LIMIT);
	if (readState)
		out.restore_rs_after_print (readState.value());
}

void Basedline::print (std::string s) {
	printQueue.push (std::move (s));
}

OptString Basedline::loop () {
	if (con.pendingResize) {
		if (con.refresh_size())
			out.scroll_and_reset (readState);
		return std::nullopt;
	}
	if (!printQueue.empty())
		do_print();
	OptString inputBuf = read_input();
	if (readState) {
		if (inputBuf)
			readState.reset();
		else if (readState->dirty)
			out.redraw_rs_from_cursor (readState.value());
		return inputBuf;
	}
	return std::nullopt;
}

Basedline::Basedline () : out (con) {
	if (!con.configure())
		std::fputs ("Failed to configure terminal", stderr);
}

Basedline::~Basedline () {
	if (!con.unconfigure())
		std::fputs ("Failed to restore terminal configuration", stderr);

#if defined(_WIN32) && defined(BASEDLINE_DEBUG)
	extern size_t csbiCalls;
	fdbg_inst().print (std::format ("CSBI calls: {}\n", csbiCalls).c_str());
#endif
}

}
