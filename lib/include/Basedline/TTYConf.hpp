#pragma once

#ifdef __WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
// bruh
#undef STRICT
#undef DELETE
#else
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
