#pragma once

#define NCURSES_WIDECHAR 1

#if defined(__WIN32)
# include <ncursesw/ncurses.h>
# undef wcwidth
# define wcwidth(ucs) _nc_wcwidth((wchar_t)(ucs))

extern "C" {
	extern NCURSES_EXPORT(int) _nc_wcwidth(uint32_t);
}

#else
# include <ncurses.h>
#endif
