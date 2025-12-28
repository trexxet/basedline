#pragma once

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

#include <string>

#include "Defs.hpp"

namespace Basedline {

class ConsoleHandle {
	friend class Cursor;
#if defined(_WIN32)
	HANDLE hIn = INVALID_HANDLE_VALUE;
	HANDLE hOut = INVALID_HANDLE_VALUE;
	DWORD hOutModeSave;
#endif
	bool raw = false;
public:
	struct RawInput {
		enum class Type { Unknown, Key, Resize } type = Type::Unknown;
#if defined(_WIN32)
		CHAR ch;
		WORD vkey;
		DWORD mods;
#endif
	};

	#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi ();
#endif
	bool enable_raw ();
	bool disable_raw ();

	bool has_input();
	RawInput get_input ();

	void putc (int c);
	void puts (const std::string& s);
	void clear_lines (termsize_t begin, termsize_t end);

	ConsoleHandle ();
	BASEDLINE_CLASS_NO_COPY_MOVE (ConsoleHandle);
};

}
