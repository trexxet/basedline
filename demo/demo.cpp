#include "Basedline/Screen.hpp"

#include <print>

int main () {
	Basedline::Screen bl;

	for (int i = 0; i < 10; i++) {
		bl.in.read();
		bl.flush();
	}

	return 0;
}
