#include "Basedline/Cursor.hpp"

#include "VtCommand.hpp"

namespace Basedline {

tpos_t Cursor::move_to (tpos_t newPos) {
	return {1, 1};
}

void Cursor::set_visible (bool val) {
	val ? VtCommand::cursor_show() : VtCommand::cursor_hide();
	_visible = val;
}

}
