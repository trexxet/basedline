#pragma once

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

#include <concepts>
#include <string>
#include <type_traits>

#include "Defs.hpp"

namespace Basedline::Console {

struct BaseHandle {
#if defined(_WIN32)
	HANDLE hOut = INVALID_HANDLE_VALUE;
	CONSOLE_SCREEN_BUFFER_INFO csbi ();
#endif
	void putc (int c);
	void puts (const std::string& s);

	BaseHandle () = default;
	BASEDLINE_CLASS_NO_COPY_MOVE (BaseHandle);
};

struct VHandle : BaseHandle {
#if defined(_WIN32)
	void sync_settings (const CONSOLE_SCREEN_BUFFER_INFO& csbi);
#endif

	VHandle ();
	~VHandle ();
	BASEDLINE_CLASS_NO_COPY_MOVE (VHandle);
};

struct OHandle : BaseHandle {
#if defined(_WIN32)
	DWORD hOutModeSave;
#endif
	bool enable_raw ();
	bool disable_raw ();

	OHandle ();
	BASEDLINE_CLASS_NO_COPY_MOVE (OHandle);
protected:
	bool raw = false;
};

struct IOHandle : OHandle {
#if defined(_WIN32)
	HANDLE hIn;
#endif

	IOHandle ();
	BASEDLINE_CLASS_NO_COPY_MOVE (IOHandle);
};

template<typename T>
concept IOClass = std::derived_from<T, BaseHandle>;

}
