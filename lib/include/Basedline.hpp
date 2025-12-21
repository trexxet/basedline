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

	void read_input ();
	void do_print ();
public:
	void read (const std::string& prompt);
	void print (std::string s);
	std::optional<std::string> loop ();

	Basedline ();
	~Basedline ();
	BASEDLINE_CLASS_NO_COPY_MOVE (Basedline);
};

}
