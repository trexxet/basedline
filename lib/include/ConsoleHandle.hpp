#pragma once

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

namespace Basedline {

struct ConsoleHandle {
#if defined(_WIN32)
	HANDLE hOut;
	DWORD hOutModeSave;
	CONSOLE_SCREEN_BUFFER_INFO csbi ();
#endif
	bool enable_raw ();
	bool disable_raw ();
	ConsoleHandle ();
protected:
	bool raw = false;
};

}
