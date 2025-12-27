#pragma once

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

#include <string>

#include "Defs.hpp"

namespace Basedline {

class ConsoleHandle {
	friend class Cursor;

public: // TODO: remove public
#if defined(_WIN32)
	HANDLE hIn = INVALID_HANDLE_VALUE;
	HANDLE hOut = INVALID_HANDLE_VALUE;
	DWORD hOutModeSave;
#endif
	bool raw = false;

public:
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi ();
#endif
	bool enable_raw ();
	bool disable_raw ();

	void putc (int c);
	void puts (const std::string& s);

	ConsoleHandle ();
	BASEDLINE_CLASS_NO_COPY_MOVE (ConsoleHandle);
};

}
