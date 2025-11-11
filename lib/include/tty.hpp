#pragma once

#include <bitset>

// TODO: Posix support
#if defined(_WIN32)
#include <windows.h>
#endif

namespace Basedline {

class TTY {
public:
	struct Input {
		enum Flags {
			OK, IS_EOF, HAS_CTRL, count
		};
		std::bitset<Flags::count> flags;
		inline bool ok ()     { return flags[Flags::OK]; }
		inline bool is_eof () { return flags[Flags::IS_EOF]; }
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

	void ctrl (Input& input, bool& skip);
	void backspace ();

public:
	bool setRaw (bool raw);

	Input getc ();
	void putc (int c);

	TTY ();
};

}
