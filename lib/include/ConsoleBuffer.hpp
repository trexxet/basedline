#pragma once

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

namespace Basedline {

struct ConsoleBuffer {
#if defined(_WIN32)
	HANDLE hOut;
	DWORD hOutModeSave;
	CONSOLE_SCREEN_BUFFER_INFO csbi ();
#endif
	bool enable_raw ();
	bool disable_raw ();
	ConsoleBuffer ();
protected:
	bool raw = false;
};

}
