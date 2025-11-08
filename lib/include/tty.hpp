#pragma once

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

namespace Basedline {

class TTY {
	bool raw = false;

#if defined(_WIN32)
	HANDLE hcon;
	DWORD hconModeSave;
#endif

	void enableRaw ();
	void disableRaw ();
public:
	void setRaw (bool raw);

	TTY ();
};

}
