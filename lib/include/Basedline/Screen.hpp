#pragma once

#include <string_view>

#include "Basedline/Cursor.hpp"
#include "Basedline/Input.hpp"
#include "Basedline/ScreenBuf.hpp"
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

class Screen {
#ifdef __WIN32
	HANDLE hOut;
#endif
	TTY tty;
	ScreenConf screenConf;
	ScreenBuf screenBuf;
	Cursor cursor;

	void clear ();
public:
	Input in;

	void write (std::string_view str);
	void flush ();

	Screen ();

	BASED_CLASS_NO_COPY_MOVE (Screen);
};

}
