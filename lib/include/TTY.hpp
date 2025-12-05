#pragma once

#include <bitset>
#include <string>

#if defined(_WIN32)
#include <windows.h>
#endif

#include "ConsoleHandle.hpp"
#include "Cursor.hpp"
#include "Defs.hpp"

namespace Basedline {

template <Console::IOClass ConsoleIOClass, Cursor::IOClass CursorIOClass>
struct ConsoleBuffer {
	ConsoleIOClass con;
	CursorIOClass cursor;

	ConsoleBuffer () : con (), cursor (con) { }
	BASEDLINE_CLASS_NO_COPY_MOVE (ConsoleBuffer);
};

class VirtualBuffer : public ConsoleBuffer<Console::VHandle, Cursor::OCursor> {
public:
	VirtualBuffer () : ConsoleBuffer () { }
	BASEDLINE_CLASS_NO_COPY_MOVE (VirtualBuffer);
};

class TTY : public ConsoleBuffer<Console::IOHandle, Cursor::IOCursor> {
public:
	struct Input {
		enum Flags {
			OK, HAS_CTRL, IS_EOF, IS_LEFT, IS_RIGHT, IS_UP, IS_DOWN, count
		};
		std::bitset<Flags::count> flags;
		inline bool ok ()       { return flags[Flags::OK]; }
		inline bool is_eof ()   { return flags[Flags::IS_EOF]; }
		inline bool is_left ()  { return flags[Flags::IS_LEFT]; }
		inline bool is_right () { return flags[Flags::IS_RIGHT]; }
		inline bool is_up ()    { return flags[Flags::IS_UP]; }
		inline bool is_down ()  { return flags[Flags::IS_DOWN]; }
#if defined(_WIN32)
		CHAR ch;
		WORD vkey;
		inline int c ()         { return static_cast<int> (ch); }
#endif
		static Input make_err () { return {}; }
	};

private:
	VirtualBuffer vbuf;

	/// @brief Process Ctrl keypress
	/// @return True if current keypress should be skipped
	bool ctrl (Input& input);
	/// @brief Process left/right arrows
	void left_right (Input& input);
	void backspace ();

public:
	bool set_raw (bool raw);

	void clear_lines (termsize_t begin, termsize_t end);

	Input getc ();
	void putc (int c, Cursor::Type curType);
	void puts (const std::string& s, Cursor::Type curType);

	TTY ();
	BASEDLINE_CLASS_NO_COPY_MOVE (TTY);
};

}
