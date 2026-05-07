#include "Basedline/Editor.hpp"

extern "C" {
#include <grapheme.h>
}

namespace Basedline {

void Editor::insert (char8_t byte) {
	buf.insert (pos, 1, byte);
	pos++;
}

void Editor::insert (std::u8string_view str) {
	buf.insert (pos, str);
	pos += str.size();
}

void Editor::move_next_grapheme () noexcept {
	if (at_end()) [[unlikely]] return;
	pos += grapheme_next_character_break_utf8 (chbuf_pos(), SIZE_MAX);
}

// TODO: maybe try optimizing for large strings
void Editor::move_prev_grapheme () noexcept {
	if (at_begin()) [[unlikely]] return;
	size_t i = 0;
	do {
		size_t new_pos_next = i + grapheme_next_character_break_utf8 (chbuf_at (i), SIZE_MAX);
		if (new_pos_next == pos) {
			pos = i;
			break;
		}
		i = new_pos_next;
	} while (i < pos);
}

void Editor::move_begin () noexcept {
	pos = 0;
}

void Editor::move_end () noexcept {
	pos = buf.size();
}

}
