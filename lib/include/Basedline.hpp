#pragma once

#include <optional>
#include <string>

#include "Basedlib/ThreadSafeQueue.hpp"

#include "ConsoleHandle.hpp"
#include "Defs.hpp"
#include "Input.hpp"
#include "ReadState.hpp"

namespace Basedline {

class Basedline {
	ConsoleHandle con;
	void print_input ();
	void restore_input ();

	std::optional<ReadState> readState;
	Basedlib::ThreadSafeQueue<std::string> printQueue;

	OptString read_input ();
	/// @return true if should continue fetching input
	bool process_input (Input& input);
	void line_edit (Input& input);

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
