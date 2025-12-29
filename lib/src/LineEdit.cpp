#include "LineEdit.hpp"

#include <bitset>
#include <cctype>

namespace Basedline::LineEdit {

bool is_lineedit (const TTY::Input& input) {
	return input.flags[TTY::Input::Flags::IS_LEFT]
	    || input.flags[TTY::Input::Flags::IS_RIGHT]
	    || std::isprint (input.c);
}

}
