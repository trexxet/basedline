#pragma once

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

#include <chrono>
#include <string_view>

#include "Defs.hpp"
#include "Input.hpp"

namespace Basedline {

class Console {
	friend class Cursor;

#if defined(_WIN32)
	HANDLE hIn = INVALID_HANDLE_VALUE;
	HANDLE hOut = INVALID_HANDLE_VALUE;
	DWORD hOutModeSave;
	bool conpty = false;
#endif

	bool configured = false;

	coord_t conSize;
	termsize_t botLine;

public:
	class Cursor {
	private:
		Console& con;
		const bool& conpty;
	public:
		coord_t pos ();
		void move (coord_t pos);
		void shift (termsize_t dx);
		coord_t wrap (termsize_t line, ssize_t pos);

		Cursor (Console& con) : con (con), conpty (con.conpty) { }
		BASEDLINE_CLASS_NO_COPY_MOVE (Cursor);
	} cursor;

	inline bool is_conpty () const { return conpty; }

#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi ();
	void set_size (const CONSOLE_SCREEN_BUFFER_INFO& csbi);
#endif
	bool configure ();
	bool unconfigure ();

	bool pendingResize = false;
	std::chrono::time_point<std::chrono::steady_clock> lastResizeReq;
	bool refresh_size();
	inline coord_t size() const { return conSize; };

	bool has_input();
	RawInput get_raw_input ();

	void putc (int c);
	void puts (std::string_view s);
	void clear_chars (size_t count);
	void clear_lines (termsize_t from, termsize_t linesToClear);

	termsize_t top_line ();
	termsize_t bottom_line ();
	termsize_t line_width ();
	termsize_t height ();

	void scroll (termsize_t linesToScroll);
	void scroll_newlines (termsize_t linesToScroll);
	bool is_last_column (termsize_t x);

	Console ();

	BASEDLINE_CLASS_NO_COPY_MOVE (Console);
};

}
