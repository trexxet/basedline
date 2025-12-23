#include "Basedline.hpp"

#include <optional>
#include <string>

int main() {
	Basedline::Basedline bl;
	std::optional<std::string> input;
	bl.read ("> ");
	bl.print ("test output\n");
	while (!(input = bl.loop()));
	bl.read ("> ");
	while (!(input = bl.loop()));
	return 0;
}
