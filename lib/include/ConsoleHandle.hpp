#pragma once

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

#include <concepts>
#include <type_traits>

#include "Defs.hpp"

namespace Basedline::Console {

struct BaseHandle {
#if defined(_WIN32)
	HANDLE hOut;
	DWORD hOutModeSave;
	CONSOLE_SCREEN_BUFFER_INFO csbi ();
#endif
	bool enable_raw ();
	bool disable_raw ();

	BaseHandle ();
	BASEDLINE_CLASS_NO_COPY_MOVE (BaseHandle);
protected:
	bool raw = false;
};

/*
struct VHandle : BaseHandle {
	BASEDLINE_CLASS_NO_COPY_MOVE (VHandle);
};

struct OHandle : BaseHandle {
	BASEDLINE_CLASS_NO_COPY_MOVE (OHandle);
};
*/
using OHandle = BaseHandle;

struct IOHandle : OHandle {
#if defined(_WIN32)
	HANDLE hIn;
#endif

	IOHandle();
	BASEDLINE_CLASS_NO_COPY_MOVE (IOHandle);
};

template<typename T>
concept IOClass = std::derived_from<T, BaseHandle>;

}
