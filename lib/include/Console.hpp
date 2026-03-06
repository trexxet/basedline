#pragma once

#include "Basedlib/Class.hpp"
#include "wrap/ncurses.h"
#include "wrap/windows.h"

#include "Defs.hpp"
#include "Input.hpp"
#include "Output.hpp"

namespace Basedline {

class Console {
#if BLWIN
	HANDLE hIn = INVALID_HANDLE_VALUE;
#endif

public:
	int acLineY; // from bottom

	InputHandler& in;
	OutputHandler& out;

	consize_t size;

	bool has_input ();
	RawInputEvent get_raw_input ();

	void sync ();

	Console (InputHandler& in, OutputHandler& out);
	~Console ();

	BASED_CLASS_NO_COPY_MOVE (Console);
};

}
