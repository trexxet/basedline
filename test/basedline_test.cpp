#include "Basedline.hpp"

int main() {
	Basedline::Basedline bl;
	bl.read ("> ");
	bl.print ("test output\n");
	bl.loop();
	bl.read ("> ");
	return 0;
}
