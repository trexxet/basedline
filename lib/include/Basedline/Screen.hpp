#pragma once

#include "Basedline/Input.hpp"
#include "Basedline/TTY.hpp"

#ifdef __WIN32
#include "Basedline/Winwrap.hpp"
#endif

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

	void clear ();
public:
	Input in;

	void render ();

	Screen ();
};

}
