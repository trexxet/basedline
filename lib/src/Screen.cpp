#include "Basedline/Screen.hpp"

#include "VtCommand.hpp"

namespace Basedline {

void Screen::clear () {
	VtCommand::clear();
}

void Screen::render () {
	VtCommand::clear_lines (0, 1);
	in.render();
}

Screen::Screen () {
	clear();
}

}
