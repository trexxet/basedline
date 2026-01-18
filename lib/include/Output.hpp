#pragma once

#include <string_view>

#include "Console.hpp"
#include "Defs.hpp"
#include "ReadState.hpp"

namespace Basedline {

struct Output {
	Console& con;
	coord_t printPos;
	termsize_t inputLine;

	void print (std::string_view s);

	void redraw_rs_from_cursor (ReadState& rs);
	void redraw_rs_with_prompt (ReadState& rs);
	void restore_rs_after_print (ReadState& rs);

	Output (Console& con);
};

}
