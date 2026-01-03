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
	bool vt = false;
#endif
	bool configured = false;

public:
	class Cursor {
	private:
		ConsoleHandle& con;
		bool& vt;
	public:
		coord_t pos ();
		void move (coord_t pos);
		void shift (termsize_t dx);

		Cursor (ConsoleHandle& con) : con (con), vt (con.vt) { }
		BASEDLINE_CLASS_NO_COPY_MOVE (Cursor);
	} cursor;

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
	bool configure ();
	bool unconfigure ();

	bool has_input();
	RawInput get_input ();

	void putc (int c);
	void puts (const std::string& s);
	void clear_lines (termsize_t from, termsize_t linesToClear);
	termsize_t bottom_line ();

	ConsoleHandle ();
	BASEDLINE_CLASS_NO_COPY_MOVE (ConsoleHandle);
};

}
