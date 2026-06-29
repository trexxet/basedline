#include "Basedline/Screen.hpp"

#include <print>

#include "VtCommand.hpp"

namespace Basedline {

#ifdef __WIN32
ScreenConf::ScreenConf (HANDLE hOut) : hOut (hOut) {
	GetConsoleMode (hOut, &modeSave);
	DWORD mode = ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode (hOut, mode)) {
		std::print (stderr, "SetConsoleMode for hOut failed\n");
		return;
	}

	cpSave = GetConsoleOutputCP();
	SetConsoleOutputCP (CP_UTF8);

	ok = true;
}

ScreenConf::~ScreenConf () {
	SetConsoleMode (hOut, modeSave);
	SetConsoleOutputCP (cpSave);
}
#endif

void Screen::clear () {
	VtCommand::clear();
}

void Screen::render () {
	VtCommand::clear_lines (0, 1);
	in.render();
}

#ifdef __WIN32
Screen::Screen () : hOut (GetStdHandle (STD_OUTPUT_HANDLE)), screenConf (hOut)
#else
Screen::Screen ()
#endif
{
	clear();
}

}
