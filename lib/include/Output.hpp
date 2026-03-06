#pragma once

#include <string>
#include <string_view>

#include "Basedlib/Class.hpp"
#include "Basedlib/RingBuffer.hpp"
#include "wrap/ncurses.h"

#include "Pad.hpp"

namespace Basedline {

class Console;

class OutputHandler {
	Console& con;
	Pad pad;
	Basedlib::RingBuffer<std::wstring> messages;

	bool lock = false;

	void slice_to_lines (const std::wstring& wcs);
public:
	void print (std::string_view s);
	void sync ();

	inline void scroll_up () { pad.scrollUp(); }
	inline void scroll_down () { pad.scrollDown(); }

	inline void toggle_lock () { lock = !lock; }

	OutputHandler (Console& con, size_t bufLines);

	BASED_CLASS_NO_COPY_MOVE (OutputHandler);
};

}
