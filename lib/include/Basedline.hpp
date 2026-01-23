#pragma once

#include <chrono>
#include <optional>
#include <string>

#include "Basedlib/ThreadSafeQueue.hpp"
#include "Basedlib/DebounceTimer.hpp"

#include "Console.hpp"
#include "Defs.hpp"
#include "Output.hpp"
#include "ReadState.hpp"

namespace Basedline {

class Basedline {
	Console con;
	Output out;
	std::optional<Basedlib::DebounceTimer> outDebounce;

	OptReadState readState;
	Basedlib::ThreadSafeQueue<std::string> printQueue;

	OptString read_input ();
	void do_print ();
public:
	bool read (const std::string& prompt);
	void print (std::string s);
	OptString loop ();

	template <typename Rep, typename Period>
	void set_print_interval (std::chrono::duration<Rep, Period> interval) {
		if (!outDebounce) outDebounce.emplace (interval);
		outDebounce->trigger();
	}

	Basedline ();
	~Basedline ();
	BASEDLINE_CLASS_NO_COPY_MOVE (Basedline);
};

}
