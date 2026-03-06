#pragma once

#include <chrono>
#include <optional>
#include <string>

#include "Basedlib/Class.hpp"
#include "Basedlib/ThreadSafeQueue.hpp"

#include "Console.hpp"
#include "Defs.hpp"
#include "Input.hpp"
#include "Output.hpp"
#include "ReadState.hpp"

namespace Basedline {

class Basedline {
	Console con;
	InputHandler in;
	OutputHandler out;

	OptReadState readState;
	Basedlib::ThreadSafeQueue<std::string> printQueue;

	OptString read_input ();

public:
	bool read (const std::string& prompt);
	void print (std::string s);
	OptString loop ();

	Basedline (size_t bufLines);
	~Basedline ();
	BASED_CLASS_NO_COPY_MOVE (Basedline);
};

}
