#pragma once

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

namespace Basedline {

class TTY {
	bool raw = false;

#if defined(_WIN32)
	HANDLE hIn, hOut;
	DWORD hInModeSave, hOutModeSave;
#endif

	bool enableRaw ();
	bool disableRaw ();
public:
	bool setRaw (bool raw);

	int getc ();
	void putc (int c);
	void cntrl (int c);

	void backspace ();

	TTY ();
};

}
