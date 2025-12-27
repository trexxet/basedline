#include "ConsoleHandle.hpp"

#include <cstdio>
#include <format>
#include <stdexcept>

#include "Debug.hpp"

namespace Basedline {

#if defined(_WIN32)
CONSOLE_SCREEN_BUFFER_INFO ConsoleHandle::csbi () {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo (hOut, &csbi)) [[unlikely]]
		throw std::runtime_error (std::format ("Can't het CSBI for ConsoleHandle hOut {}", hOut));
	return csbi;
}
#endif

bool ConsoleHandle::enable_raw () {
	if (raw) return true;
#if defined(_WIN32)
	DWORD rawOutMode = hOutModeSave |
		ENABLE_PROCESSED_OUTPUT |
		ENABLE_WRAP_AT_EOL_OUTPUT;
	raw = SetConsoleMode (hOut, rawOutMode);
	return raw;
#endif
}

bool ConsoleHandle::disable_raw () {
	if (!raw) return true;
#if defined(_WIN32)
	raw = !SetConsoleMode (hOut, hOutModeSave);
	return !raw;
#endif
}

void ConsoleHandle::putc (int c) {
#if defined(_WIN32)
	if (c == EOF) return;
	char ch = (char) c; 
	WriteConsole (hOut, &ch, 1, NULL, NULL);
#else
	std::fputc (c, stdout);
#endif
}

void ConsoleHandle::puts (const std::string& s) {
#if defined(_WIN32)
	WriteConsole (hOut, s.c_str(), s.length(), NULL, NULL);
#else
	std::printf (s.c_str());
#endif
}

ConsoleHandle::ConsoleHandle () {
#if defined(_WIN32)
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hOut, &hOutModeSave);
	BL_DEBUG ("New ConsoleHandle hOut {} hIn {}\n", hOut, hIn);
#endif
}

}
