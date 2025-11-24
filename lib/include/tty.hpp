#pragma once

#include <bitset>
#include <string>

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

namespace Basedline {

#if defined(_WIN32)
using termsize_t = SHORT;
using coord_t = COORD;
#endif

struct ConsoleBuffer {
#if defined(_WIN32)
	HANDLE hIn, hOut;
	DWORD hInModeSave, hOutModeSave;
	CONSOLE_SCREEN_BUFFER_INFO csbi ();
#endif
	bool enable_raw ();
	bool disable_raw ();
	ConsoleBuffer ();
protected:
	bool raw = false;
};

class Cursor {
public:
	enum Type { CurInput, CurOutput, count };
protected:
	ConsoleBuffer& tty;
	Type currType;
	coord_t pos[Type::count];
public:
	void save ();
	void set (Type type);
	void move (coord_t pos);
	Cursor (ConsoleBuffer& tty);
};

class VirtualBuffer : public ConsoleBuffer {
	Cursor cursor;
public:
	VirtualBuffer () : ConsoleBuffer(), cursor (*this) { }
};

class TTY : private ConsoleBuffer {
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

	class TTYCursor : public Cursor {
	public:
		void input_shift (termsize_t dx, termsize_t left_constraint);
		void input_move_down ();
	} cursor;

private:
	VirtualBuffer vbuf;

	std::string prompt;

	/// @brief Process Ctrl keypress
	/// @return True if current keypress should be skipped
	bool ctrl (Input& input);
	/// @brief Process left/right arrows
	void left_right (Input& input);
	void backspace ();

public:
	bool set_raw (bool raw);
	void set_prompt (const std::string& prompt);

	Input getc ();
	void putc (int c, Cursor::Type curType);
	void puts (const std::string& s, Cursor::Type curType);

	TTY () : ConsoleBuffer(), cursor (*this) { }
};

}
