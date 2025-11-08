#include "tty.hpp"
// TODO: Posix support

namespace Basedline {

void TTY::enableRaw () {
	if (raw) return;
#if defined(_WIN32)
	DWORD rawMode = hconModeSave & ~(
		ENABLE_ECHO_INPUT |
		ENABLE_LINE_INPUT
	);
	SetConsoleMode (hcon, rawMode);
#endif
}

void TTY::disableRaw () {
	if (!raw) return;
#if defined(_WIN32)
	SetConsoleMode (hcon, hconModeSave);
#endif
}

void TTY::setRaw (bool raw) {
	raw ? enableRaw() : disableRaw();
	this->raw = raw;
}

TTY::TTY () {
#if defined(_WIN32)
	hcon = GetStdHandle (STD_INPUT_HANDLE);
	GetConsoleMode (hcon, &hconModeSave);
#endif
}

}
