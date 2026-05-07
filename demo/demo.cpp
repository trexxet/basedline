#include "Basedline/Editor.hpp"

#include <print>

int main () {
	Basedline::Editor ed;

	ed.insert ('a');
	ed.insert (u8"ы");
	ed.insert (u8"🙂界𒁲");

	std::print ("{}\n", ed.get());

	ed.move_begin();
	while (!ed.at_end()) {
		std::print ("{}\n", ed.get_pos());
		ed.move_next_grapheme();
	}

	while (!ed.at_begin()) {
		ed.move_prev_grapheme();
		std::print ("{}\n", ed.get_pos());
	}

	return 0;
}
