#include "Basedline/Screen.hpp"

#include <print>

int main () {
	Basedline::Screen bl;

	bl.in.read();
	bl.render();

	return 0;
}
