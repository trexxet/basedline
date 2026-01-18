#pragma once

#include <string>

#include "Basedlib/ThreadSafeQueue.hpp"

#include "Console.hpp"
#include "Defs.hpp"
#include "Output.hpp"
#include "ReadState.hpp"

namespace Basedline {

class Basedline {
	Console con;
	Output out;

	OptReadState readState;
	Basedlib::ThreadSafeQueue<std::string> printQueue;

	OptString read_input ();
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
