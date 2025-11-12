#pragma once

#include <bitset>
#include <string_view>

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

namespace Basedline {

class TTY {
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
		inline int c ()       { return static_cast<int> (ch); }
#endif
		static Input make_err () { return {}; }
	};

private:
	bool raw = false;

#if defined(_WIN32)
	HANDLE hIn, hOut;
	DWORD hInModeSave, hOutModeSave;
#endif

	bool enableRaw ();
	bool disableRaw ();

	void moveCursor (short dx);
	void moveCursor (short x, short y);

	/// @brief Process Ctrl keypress
	/// @return True if current keypress should be skipped
	bool ctrl (Input& input);
	/// @brief Process left/right arrows
	void left_right (Input& input);
	void backspace ();

public:
	bool setRaw (bool raw);

	Input getc ();
	void putc (int c);

	std::string_view prompt;

	TTY ();
};

}
