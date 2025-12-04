#include "ConsoleHandle.hpp"

namespace Basedline::Console {

#if defined(_WIN32)
CONSOLE_SCREEN_BUFFER_INFO BaseHandle::csbi () {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo (hOut, &csbi);
	return csbi;
}
#endif

#if defined(_WIN32)
void VHandle::sync_settings (const CONSOLE_SCREEN_BUFFER_INFO& csbi) {
	SetConsoleScreenBufferSize (hOut, csbi.dwSize);
	SetConsoleTextAttribute (hOut, csbi.wAttributes);
}
#endif

VHandle::VHandle () {
#if defined(_WIN32)
	hOut = CreateConsoleScreenBuffer (GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
#endif
}

VHandle::~VHandle () {
#if defined(_WIN32)
	CloseHandle (hOut);
#endif
}

bool OHandle::enable_raw () {
	if (raw) return true;
#if defined(_WIN32)
	DWORD rawOutMode = hOutModeSave |
		ENABLE_PROCESSED_OUTPUT |
		ENABLE_WRAP_AT_EOL_OUTPUT;
	raw = SetConsoleMode (hOut, rawOutMode);
	return raw;
#endif
}

bool OHandle::disable_raw () {
	if (!raw) return true;
#if defined(_WIN32)
	raw = !SetConsoleMode (hOut, hOutModeSave);
	return !raw;
#endif
}

OHandle::OHandle () {
#if defined(_WIN32)
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hOut, &hOutModeSave);
#endif
}

IOHandle::IOHandle () {
#if defined(_WIN32)
	hIn = GetStdHandle (STD_INPUT_HANDLE);
#endif
}

}
