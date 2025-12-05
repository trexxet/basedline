#include "ConsoleHandle.hpp"

#include <format>
#include <stdexcept>

#if defined(BASEDLINE_DEBUG)
# include "Debug.hpp"
#endif

#include <cstdio>

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
	GetConsoleScreenBufferInfo (hOut, &csbi);
	return csbi;
}
#endif

#if defined(_WIN32)
void VHandle::sync_settings (const CONSOLE_SCREEN_BUFFER_INFO& csbi) {
	if (!SetConsoleScreenBufferSize (hOut, csbi.dwSize) || !SetConsoleTextAttribute (hOut, csbi.wAttributes))
		throw std::runtime_error (std::format ("Can't sync settings for VHandle hOut {}", hOut));
# if defined(BASEDLINE_DEBUG)
	Debug::print (std::format ("VHandle hOut {} sync dwSize.X {} dwSize.Y {}\n", hOut, csbi.dwSize.X, csbi.dwSize.Y));
# endif
}
#endif

VHandle::VHandle () {
#if defined(_WIN32)
	hOut = CreateConsoleScreenBuffer (GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (hOut == INVALID_HANDLE_VALUE)
		throw std::runtime_error ("Can't create hOut for VHandle");
# if defined(BASEDLINE_DEBUG)
	Debug::print (std::format ("New VHandle hOut {}\n", hOut));
# endif
#endif
}

VHandle::~VHandle () {
#if defined(_WIN32)
# if defined(BASEDLINE_DEBUG)
	Debug::print (std::format ("Closing VHandle hOut {}\n", hOut));
# endif
	CloseHandle (hOut);
#endif
}

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
# if defined(BASEDLINE_DEBUG)
	Debug::print (std::format ("New OHandle hOut {}\n", hOut));
# endif
#endif
}

IOHandle::IOHandle () {
#if defined(_WIN32)
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	Debug::print (std::format ("New IOHandle hOut {} hIn {}\n", hOut, hIn));
#endif
}

}
