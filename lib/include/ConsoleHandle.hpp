#pragma once

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

#include <concepts>
#include <type_traits>

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
protected:
	bool raw = false;
};

using OHandle = BaseHandle;

struct IOHandle : BaseHandle {
#if defined(_WIN32)
	HANDLE hIn;
#endif
	IOHandle();
};

template<typename T>
concept IOClass = std::derived_from<T, BaseHandle>;

}
