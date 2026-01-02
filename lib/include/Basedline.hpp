#pragma once

#include <optional>
#include <string>

#include "Basedlib/ThreadSafeQueue.hpp"
#include "Defs.hpp"
#include "ReadState.hpp"
#include "TTY.hpp"

namespace Basedline {

class Basedline {
	TTY tty;
	void restore_input ();

	std::optional<ReadState> readState;
	Basedlib::ThreadSafeQueue<std::string> printQueue;

	OptString read_input ();
	/// @return true if should continue fetching input
	bool process_input (TTY::Input& input);
	void line_edit(TTY::Input& input);

	coord_t outputPos;
	void do_print ();
public:
	bool read (const std::string& prompt);
	void print (std::string s);
	OptString loop ();

	Basedline ();
	~Basedline ();
	BASEDLINE_CLASS_NO_COPY_MOVE (Basedline);
};

}
