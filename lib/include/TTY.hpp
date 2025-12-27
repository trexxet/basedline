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

struct ConsoleBuffer {
	ConsoleHandle con;
	Cursor cursor;

	ConsoleBuffer () : con (), cursor (con) { }
	BASEDLINE_CLASS_NO_COPY_MOVE (ConsoleBuffer);
};

class TTY : public ConsoleBuffer {
public:
	struct Input {
		enum Flags {
			OK, HAS_CTRL, IS_EOL, IS_LEFT, IS_RIGHT, IS_UP, IS_DOWN, count
		};
		std::bitset<Flags::count> flags;
		inline bool ok ()       { return flags[Flags::OK]; }
		inline bool is_eol ()   { return flags[Flags::IS_EOL]; }
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
	void ctrl (Input& input);
	void left_right (Input& input);

public:
	bool set_raw (bool raw);

	bool has_input ();
	Input getc ();
	void putc (int c, Cursor::Type curType);
	void puts (const std::string& s, Cursor::Type curType);
	void clear_lines (termsize_t begin, termsize_t end);

	TTY () { }
	BASEDLINE_CLASS_NO_COPY_MOVE (TTY);
};

}
