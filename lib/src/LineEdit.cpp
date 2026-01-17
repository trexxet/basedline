#include "LineEdit.hpp"

#include <bitset>

namespace Basedline::LineEdit {

void apply_lr (KeyInput& input, ReadState& rs) {
	if (input.is_left()) {
		if (rs.linebufCursor > 0) rs.linebufCursor--;
		else input.flags[KeyInput::Flags::IS_LEFT] = false;
	} else if (input.is_right()) {
		if (rs.linebufCursor < rs.linebuf.length()) rs.linebufCursor++;
		else input.flags[KeyInput::Flags::IS_RIGHT] = false;
	}
}

void apply_bkspc (KeyInput& input, ReadState& rs) {
	if (rs.linebufCursor > 0) {
		rs.linebuf.erase (rs.linebufCursor - 1, 1);
		rs.linebufCursor--;
		rs.dirty = true;
		rs.deletes++;
	} else input.flags[KeyInput::Flags::IS_BKSPC] = false;
}

void apply_print (KeyInput& input, ReadState& rs) {
	// TODO: add reserve for linebuf
	if (rs.linebufCursor == rs.linebuf.length())
		rs.linebuf += input.c;
	else
		rs.linebuf.insert (rs.linebufCursor, 1, input.c);
	rs.linebufCursor++;
	rs.dirty = true;
}

void apply_del (KeyInput& input, ReadState& rs) {
	if (rs.linebufCursor < rs.linebuf.length()) {
		rs.linebuf.erase (rs.linebufCursor, 1);
		rs.dirty = true;
		rs.deletes++;
	} else input.flags[KeyInput::Flags::IS_DEL] = false;
}

bool is_lineedit (const KeyInput& input) {
	return input.is_lr()
	    || input.is_bkspc()
	    || input.is_del()
	    || input.is_print();
}

void apply (KeyInput& input, ReadState& rs) {
	// TODO: switch/case?
	if (input.is_lr())    return apply_lr (input, rs);
	if (input.is_bkspc()) return apply_bkspc (input, rs);
	if (input.is_del())   return apply_del (input, rs);
	if (input.is_print()) return apply_print (input, rs);
}

}
