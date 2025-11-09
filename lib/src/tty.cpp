#include "tty.hpp"
// TODO: Posix support

#include <cstdio>

#if defined(_WIN32)
#define ESC "\x1b"
#define CSI ESC "["
#endif

namespace Basedline {

bool TTY::enableRaw () {
	if (raw) return true;
#if defined(_WIN32)
	DWORD rawInMode = hInModeSave & ~(
		ENABLE_ECHO_INPUT |
		ENABLE_LINE_INPUT |
		ENABLE_PROCESSED_INPUT
	);
	DWORD rawOutMode = hOutModeSave |
		ENABLE_PROCESSED_OUTPUT |
		ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	return SetConsoleMode (hIn, rawInMode) & SetConsoleMode (hOut, rawOutMode);
#endif
}

bool TTY::disableRaw () {
	if (!raw) return true;
#if defined(_WIN32)
	return SetConsoleMode (hIn, hInModeSave) & SetConsoleMode (hOut, hOutModeSave);
#endif
}

bool TTY::setRaw (bool raw) {
	bool isSet = raw ? enableRaw() : disableRaw();
	this->raw = raw;
	return isSet;
}

int TTY::getc () {
	return std::fgetc (stdin);
}

void TTY::putc (int c) {
	std::fputc (c, stdout);
}

void TTY::cntrl (int c) {
	switch (c) {
		case '\b':
			backspace(); break;
		default: break;
	}
}

void TTY::backspace () {
	std::printf (CSI "0M");
}

TTY::TTY () {
#if defined(_WIN32)
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hIn, &hInModeSave);
	GetConsoleMode (hOut, &hOutModeSave);
#endif
}

}
