#pragma once

#ifdef __WIN32
#include <windows.h>
#include <io.h>
// bruh
#undef STRICT
#undef DELETE
#else
#include <unistd.h>
#include <termios.h>
#endif

namespace Basedline {

#ifdef __WIN32
struct TTYConf {
	HANDLE hIn;
	DWORD modeSave;

	bool ok = false;
	TTYConf();
	~TTYConf();
};
#else
struct TTYConf {
	termios save;

	bool saved = false, ok = false;
	TTYConf();
	~TTYConf();
};
#endif

}
