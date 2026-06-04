#include "Basedline/TTYConf.hpp"

#include <cstdio>
#include <print>

namespace Basedline {

#ifdef __WIN32
TTYConf::TTYConf () {
	hIn = GetStdHandle (STD_INPUT_HANDLE);
	GetConsoleMode (hIn, &modeSave);
	DWORD mode = modeSave | ENABLE_VIRTUAL_TERMINAL_INPUT;
	mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
	if (!SetConsoleMode (hIn, mode)) {
		std::print ("SetConsoleMode failed\n");
		return;
	}
	ok = true;
}

TTYConf::~TTYConf () {
	SetConsoleMode (hIn, modeSave);
}
#else
TTYConf::TTYConf () {
	if (!isatty (STDIN_FILENO)) {
		std::print (stderr, "stdin is not a tty\n");
		return;
	}
	if (tcgetattr (STDIN_FILENO, &save) != 0) {
		std::perror ("tcgetattr");
		return;
	}

	saved = true;
	termios cfg = save;
	cfg.c_lflag &= ~(ICANON | ISIG | ECHO);
	cfg.c_iflag &= ~(IXON);
	cfg.c_cc[VMIN] = 1;
	cfg.c_cc[VTIME] = 0;

	if (tcsetattr (STDIN_FILENO, TCSAFLUSH, &cfg) != 0) {
		std::perror ("tcsetattr");
		return;
	}
	ok = true;
}

TTYConf::~TTYConf () {
	if (!saved) return;
	if (tcsetattr (STDIN_FILENO, TCSANOW, &save) != 0)
		std::perror ("tcsetattr");
}
#endif

}
