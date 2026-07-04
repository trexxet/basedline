#pragma once

#include <string>
#include <string_view>

#include "Basedline/Cursor.hpp"
#include "Basedline/Input.hpp"
#include "Basedline/TTY.hpp"

#ifdef __WIN32
#include "Basedline/Winwrap.hpp"
#endif

#include "Basedlib/Class.hpp"

namespace Basedline {

#ifdef __WIN32
struct ScreenConf {
	HANDLE hOut;
	DWORD modeSave;
	UINT cpSave;

	bool ok = false;
	ScreenConf (HANDLE hOut);
	~ScreenConf ();
};
#else
struct ScreenConf {};
#endif

class ScreenBuf {
	const size_t defaultSize = 1024;
	const size_t maxSize = 16 * defaultSize;
	std::string buf;
public:
	void push (std::string_view val);
	void flush ();

	ScreenBuf ();
};

class Screen {
#ifdef __WIN32
	HANDLE hOut;
#endif
	TTY tty;
	ScreenConf screenConf;
	Cursor cursor;

	void clear ();
public:
	Input in;

	void flush ();

	Screen ();

	BASED_CLASS_NO_COPY_MOVE (Screen);
};

}
