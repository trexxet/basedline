#pragma once

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

#include <string_view>

#include "Defs.hpp"
#include "Input.hpp"

namespace Basedline {

class ConsoleHandle {
	friend class Cursor;
#if defined(_WIN32)
	HANDLE hIn = INVALID_HANDLE_VALUE;
	HANDLE hOut = INVALID_HANDLE_VALUE;
	DWORD hOutModeSave;
	bool conpty = false;
	bool vt = false;
#endif
	bool configured = false;

public:
	class Cursor {
	private:
		ConsoleHandle& con;
		const bool& conpty;
		const bool& vt;
	public:
		coord_t pos ();
		void move (coord_t pos);
		void shift (termsize_t dx);
		coord_t wrap (termsize_t line, ssize_t pos);

		Cursor (ConsoleHandle& con) : con (con), vt (con.vt), conpty (con.conpty) { }
		BASEDLINE_CLASS_NO_COPY_MOVE (Cursor);
	} cursor;

	inline bool is_conpty () const { return conpty; }
	inline bool is_vt () const { return vt; }

#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi ();
#endif
	bool configure ();
	bool unconfigure ();

	bool has_input();
	RawInput get_raw_input ();

	void putc (int c);
	void puts (std::string_view s);
	void clear_chars (size_t count);
	void clear_lines (termsize_t from, termsize_t linesToClear);
	termsize_t bottom_line ();
	termsize_t line_width ();
	void scroll (termsize_t linesToScroll);
	bool is_last_column (termsize_t x);

	ConsoleHandle ();
	BASEDLINE_CLASS_NO_COPY_MOVE (ConsoleHandle);
};

}
