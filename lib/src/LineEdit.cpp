#include "LineEdit.hpp"

#include <bitset>
#include <cctype>

namespace Basedline::LineEdit {

bool is_lineedit (const Input& input) {
	return input.is_lr() || std::isprint (input.c);
}

void apply (Input& input, ReadState& rs) {
	if (input.is_lr())
		return apply_lr (input, rs);
	if (std::isprint (input.c)) {
		// TODO: add reserve for linebuf
		if (rs.linebufCursor == rs.linebuf.length())
			rs.linebuf += input.c;
		else
			rs.linebuf.insert (rs.linebufCursor, 1, input.c);
		rs.linebufCursor++;
		rs.dirty = true;
	}
}

void apply_lr (Input& input, ReadState& rs) {
	if (input.is_left()) {
		if (rs.linebufCursor > 0) rs.linebufCursor--;
		else input.flags[Input::Flags::IS_LEFT] = false;
	} else if (input.is_right()) {
		if (rs.linebufCursor < rs.linebuf.length()) rs.linebufCursor++;
		else input.flags[Input::Flags::IS_RIGHT] = false;
	}
}

}
