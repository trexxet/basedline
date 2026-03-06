#include "Console.hpp"

#include <clocale>
#include <format>
#include <stdexcept>

#include "Debug.hpp"

#define BL_MIN_HEIGHT 8
#define BL_MIN_WIDTH 32
#define BL_DEFAULT_AC_LINE_Y 3 // from bottom

namespace Basedline {

bool Console::has_input () {
#if BLWIN
	DWORD input_events = 0;
	if (!GetNumberOfConsoleInputEvents (hIn, &input_events))
		return false;
	return (input_events > 0);
#else
	// select
#endif
}

RawInputEvent Console::get_raw_input () {
	RawInputEvent rawInput;
#if BLWIN
	INPUT_RECORD inputRec;
	DWORD inputCount;

	if (!ReadConsoleInput (hIn, &inputRec, 1, &inputCount) || inputCount != 1)
		return rawInput;

	switch (inputRec.EventType) {
		[[likely]] case KEY_EVENT:
			if (!inputRec.Event.KeyEvent.bKeyDown) break;
			rawInput.type = RawInputEvent::Type::Key;
			rawInput.ev.key = {
				.ch   = inputRec.Event.KeyEvent.uChar.AsciiChar,
				.vkey = inputRec.Event.KeyEvent.wVirtualKeyCode,
				.mods = inputRec.Event.KeyEvent.dwControlKeyState
			};
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
			rawInput.type = RawInputEvent::Type::Resize;
			//rawInput.ev.newSize = inputRec.Event.WindowBufferSizeEvent.dwSize;
			break;
		default: break;
	}
#else
	// linux idk
#endif
	return rawInput;
}

void Console::sync () {
	in.sync ();
	out.sync ();
	wnoutrefresh (stdscr);
	doupdate();
}

Console::Console (InputHandler& in, OutputHandler& out) : acLineY (BL_DEFAULT_AC_LINE_Y), in (in), out (out) {
	std::setlocale (LC_ALL, "C.utf8");

	initscr();
	noecho();
	curs_set (0);

	getmaxyx (stdscr, size.y, size.x);
	if (size.x < BL_MIN_WIDTH || size.y < BL_MIN_HEIGHT)
		throw std::runtime_error (std::format ("Terminal size [{} {}] is less than minimal [{} {}]",
			size.x, size.y, BL_MIN_WIDTH, BL_MIN_HEIGHT));

	wnoutrefresh (stdscr);

#if BLWIN
	hIn = GetStdHandle (STD_INPUT_HANDLE);
#endif

}

Console::~Console () {
	endwin();
}

}
