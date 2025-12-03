#include "ConsoleBuffer.hpp"

namespace Basedline {

#if defined(_WIN32)
CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer::csbi () {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo (hOut, &csbi);
	return csbi;
}
#endif

bool ConsoleBuffer::enable_raw () {
	if (raw) return true;
#if defined(_WIN32)
	DWORD rawOutMode = hOutModeSave |
		ENABLE_PROCESSED_OUTPUT |
		ENABLE_WRAP_AT_EOL_OUTPUT;
	raw = SetConsoleMode (hOut, rawOutMode);
	return raw;
#endif
}

bool ConsoleBuffer::disable_raw () {
	if (!raw) return true;
#if defined(_WIN32)
	raw = !SetConsoleMode (hOut, hOutModeSave);
	return !raw;
#endif
}

ConsoleBuffer::ConsoleBuffer () {
#if defined(_WIN32)
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hOut, &hOutModeSave);
#endif
}

}
