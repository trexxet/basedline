#include "ConsoleHandle.hpp"

#include <cstdio>
#include <format>
#include <stdexcept>

#include "Debug.hpp"

namespace Basedline::Console {

void BaseHandle::putc (int c) {
#if defined(_WIN32)
	if (c == EOF) return;
	char ch = (char) c; 
	WriteConsole (hOut, &ch, 1, NULL, NULL);
#else
	std::fputc (c, stdout);
#endif
}

void BaseHandle::puts (const std::string& s) {
#if defined(_WIN32)
	WriteConsole (hOut, s.c_str(), s.length(), NULL, NULL);
#else
	std::printf (s.c_str());
#endif
}

#if defined(_WIN32)
CONSOLE_SCREEN_BUFFER_INFO BaseHandle::csbi () {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo (hOut, &csbi)) [[unlikely]]
		throw std::runtime_error (std::format ("Can't het CSBI for BaseHandle hOut {}", hOut));
	return csbi;
}
#endif

bool OHandle::enable_raw () {
	if (raw) return true;
#if defined(_WIN32)
	DWORD rawOutMode = hOutModeSave |
		ENABLE_PROCESSED_OUTPUT |
		ENABLE_WRAP_AT_EOL_OUTPUT;
	raw = SetConsoleMode (hOut, rawOutMode);
	return raw;
#endif
}

bool OHandle::disable_raw () {
	if (!raw) return true;
#if defined(_WIN32)
	raw = !SetConsoleMode (hOut, hOutModeSave);
	return !raw;
#endif
}

OHandle::OHandle () {
#if defined(_WIN32)
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleMode (hOut, &hOutModeSave);
	BL_DEBUG ("New OHandle hOut {}\n", hOut);
#endif
}

IOHandle::IOHandle () {
#if defined(_WIN32)
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	BL_DEBUG ("New IOHandle hOut {} hIn {}\n", hOut, hIn);
#endif
}

}
