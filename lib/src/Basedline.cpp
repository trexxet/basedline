#include "Basedline.hpp"

#include "Debug.hpp"

#define BL_MAX_READ_LIMIT 256

namespace Basedline {

OptString Basedline::read_input () {
	size_t read = 0;
	while (con.has_input() && read < BL_MAX_READ_LIMIT) {
		InputEvent input = InputEvent::get (con);
		if (!input.process (con, readState) && readState) {
			//print (std::move (out.echo_rs (readState.value())));
			return std::move (readState->linebuf);
		}
		read++;
	}
	return std::nullopt;
}

bool Basedline::read (const std::string& prompt) {
	if (readState) return false;
	readState.emplace (prompt);
	//out.redraw_rs_with_prompt (readState.value());
	return true;
}

void Basedline::print (std::string s) {
	printQueue.push (std::move (s));
}

OptString Basedline::loop () {
/*
	if (con.pendingResize) {
		if (con.refresh_size())
			out.scroll_and_reset (readState);
		return std::nullopt;
	}
*/
	while (!printQueue.empty()) {
		out.print (printQueue.pop().value());
	}

	OptString inputBuf = read_input();
	if (readState) {
		if (inputBuf)
			readState.reset();
	}

	con.sync();

	return inputBuf;
}

Basedline::Basedline (size_t bufLines) :  con (in, out), in (con, bufLines), out (con, bufLines) {
/*
	if (!con.configure())
		std::fputs ("Failed to configure terminal", stderr);
*/
}

Basedline::~Basedline () {
/*
	if (!con.unconfigure())
		std::fputs ("Failed to restore terminal configuration", stderr);

#if defined(_WIN32) && defined(BASEDLINE_DEBUG)
	extern size_t csbiCalls;
	fdbg_inst().print (std::format ("CSBI calls: {}\n", csbiCalls).c_str());
#endif
*/
}

}
