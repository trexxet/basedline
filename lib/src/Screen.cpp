#include "Basedline/Screen.hpp"

#include <cstdio>
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

void ScreenBuf::push (std::string_view val) {
	buf.append (val);
}

void ScreenBuf::flush () {
	if (buf.empty()) [[unlikely]] return;

	std::fwrite (buf.data(), 1, buf.size(), stdout);
	std::fflush (stdout);

	buf.clear();
	if (buf.capacity() > maxSize) {
		std::string tmp;
		tmp.reserve (defaultSize);
		buf.swap (tmp);
	}
}

ScreenBuf::ScreenBuf () {
	buf.reserve (defaultSize);
}

void Screen::clear () {
	VtCommand::clear();
}

void Screen::flush () {
	cursor.set_visible (false);
	VtCommand::clear_lines (0, 1);
	in.render();
	cursor.set_visible (true);
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
