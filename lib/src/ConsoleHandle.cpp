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
	if (!WriteConsole (hOut, s.c_str(), s.length(), NULL, NULL)) [[unlikely]]
		throw std::runtime_error (std::format ("Can't WriteConsole for BaseHandle hOut {}", hOut));
#else
	std::printf (s.c_str());
#endif
}

#if defined(_WIN32)
CONSOLE_SCREEN_BUFFER_INFO BaseHandle::csbi () {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo (hOut, &csbi)) [[unlikely]]
		throw std::runtime_error (std::format ("Can't get CSBI for BaseHandle hOut {}", hOut));
	return csbi;
}
#endif

#if defined(_WIN32)
// TODO: what if we sync the whole CSBI?
void VHandle::sync_settings (const CONSOLE_SCREEN_BUFFER_INFO& csbi) {
	CONSOLE_SCREEN_BUFFER_INFO curr_csbi = this->csbi();

	// 1) dwSize can't be less than srWindow, so if window is shrinked, should
	// set srWindow to 0 first
	bool shrink = (csbi.dwSize.X < (curr_csbi.srWindow.Right - curr_csbi.srWindow.Left + 1))
	           || (csbi.dwSize.Y < (curr_csbi.srWindow.Bottom - curr_csbi.srWindow.Top + 1));
	if (shrink) {
		SMALL_RECT shrinkWindow {0, 0, 0, 0};
		if (!SetConsoleWindowInfo (hOut, TRUE, &shrinkWindow))
			throw std::runtime_error (std::format ("Can't shrink window for VHandle hOut {}", hOut));
		Debug::print (std::format ("VHandle hOut {} shrinked window\n", hOut));
	}

	// 2) srWindow can't be bigger than dwSize, so resize dwSize first
	if (!SetConsoleScreenBufferSize (hOut, csbi.dwSize)
	 || !SetConsoleWindowInfo (hOut, TRUE, &csbi.srWindow)
	 || !SetConsoleTextAttribute (hOut, csbi.wAttributes)) [[unlikely]]
		throw std::runtime_error (std::format ("Can't sync settings for VHandle hOut {}", hOut));

# if defined(BASEDLINE_DEBUG)
	Debug::print (std::format ("VHandle hOut {} sync dwSize [{} {}]\n", hOut, csbi.dwSize.X, csbi.dwSize.Y));
# endif
}
#endif

VHandle::VHandle () {
#if defined(_WIN32)
	hOut = CreateConsoleScreenBuffer (GENERIC_WRITE | GENERIC_READ, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (hOut == INVALID_HANDLE_VALUE) [[unlikely]]
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
	if (!GetConsoleMode (hOut, &hOutModeSave)) [[unlikely]]
		throw std::runtime_error (std::format ("Can't GetConsoleMode for OHandle hOut {}", hOut));
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
