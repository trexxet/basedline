#pragma once

#include <optional>
#include <string>

#include "Basedlib/ThreadSafeQueue.hpp"

#include "Console.hpp"
#include "Defs.hpp"
#include "Input.hpp"
#include "Output.hpp"
#include "ReadState.hpp"

namespace Basedline {

class Basedline {
	Console con;
	Output out;

	std::optional<ReadState> readState;
	Basedlib::ThreadSafeQueue<std::string> printQueue;

	OptString read_input ();
	/// @return true if should continue fetching input
	bool process_input (Input& input);
	bool process_key_input (KeyInput& kinput);
	void line_edit (KeyInput& input);

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
