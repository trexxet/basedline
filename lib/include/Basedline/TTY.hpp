#pragma once

#ifdef __WIN32
#include "Basedline/Winwrap.hpp"
#else
#include <termios.h>
#endif

#include "Basedlib/Class.hpp"

namespace Basedline {

#ifdef __WIN32
struct TTYConf {
	HANDLE hIn;
	DWORD modeSave;

	bool ok = false;
	TTYConf (HANDLE hIn);
	~TTYConf ();
};
#else
struct TTYConf {
	termios save;

	bool saved = false, ok = false;
	TTYConf ();
	~TTYConf ();
};
#endif

class TTY {
#ifdef __WIN32
	HANDLE hIn;
#endif
	TTYConf ttyConf;
public:
	TTY ();

	BASED_CLASS_NO_COPY_MOVE (TTY);
};

}
