#include "Basedline/Editor.hpp"

extern "C" {
#include <grapheme.h>
}

namespace Basedline {

void Editor::clear () noexcept {
	acc.clear();
	buf.clear();
	pos = 0;
}

void Editor::accumulate (char8_t byte) {
	if (byte == 0) [[unlikely]] {
		acc.clear();
		return;
	}

	acc.emplace_back (byte);
	uint_least32_t cp = GRAPHEME_INVALID_CODEPOINT;
	size_t processed = grapheme_decode_utf8 (acc.data(), acc.size(), &cp);

	if (cp == GRAPHEME_INVALID_CODEPOINT) {
		if (processed <= acc.size()) // invalid
			acc.clear();
	} else {
		insert ({reinterpret_cast <const char8_t*> (acc.data()), acc.size()});
		acc.clear();
	}
}

void Editor::insert (std::u8string_view str) {
	acc.clear();
	buf.insert (pos, str);
	pos += str.size();
}

void Editor::move_next_grapheme () noexcept {
	acc.clear();
	if (at_end()) [[unlikely]] return;
	pos += grapheme_next_character_break_utf8 (chbuf_pos(), SIZE_MAX);
}

// TODO: maybe try optimizing for large strings
void Editor::move_prev_grapheme () noexcept {
	acc.clear();
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

static bool move_word_skip (char8_t c) noexcept {
	if (c >= 0x80) return false;
	if (c == u8'_') return false;
	return (c == u8' ' || c == u8'\t')
	    || (c >= u8'!' && c <= u8'/')
	    || (c >= u8':' && c <= u8'@')
	    || (c >= u8'[' && c <= u8'`')
	    || (c >= u8'{' && c <= u8'~');
}

void Editor::move_next_word () noexcept {
	acc.clear();
	if (at_end()) [[unlikely]] return;
	do {
		pos += grapheme_next_word_break_utf8 (chbuf_pos(), SIZE_MAX);
	} while (!at_end() && move_word_skip (*chbuf_pos_u8()));
}

// TODO: maybe try optimizing for large strings
void Editor::move_prev_word () noexcept {
	acc.clear();
	if (at_begin()) [[unlikely]] return;
	do {
		size_t i = 0;
		do {
			size_t new_pos_next = i + grapheme_next_word_break_utf8 (chbuf_at (i), SIZE_MAX);
			if (new_pos_next >= pos) {
				pos = i;
				break;
			}
			i = new_pos_next;
		} while (i < pos);
	} while (!at_begin() && move_word_skip (*chbuf_pos_u8()));
}

void Editor::move_begin () noexcept {
	acc.clear();
	pos = 0;
}

void Editor::move_end () noexcept {
	acc.clear();
	pos = buf.size();
}

void Editor::erase_pos_grapheme () noexcept {
	acc.clear();
	if (at_end()) [[unlikely]] return;

	const size_t pos_save = pos;
	move_next_grapheme();
	buf.erase (pos_save, pos - pos_save);
	pos = pos_save;
}

void Editor::erase_prev_grapheme () noexcept {
	acc.clear();
	if (at_begin()) [[unlikely]] return;

	const size_t pos_save = pos;
	move_prev_grapheme();
	buf.erase (pos, pos_save - pos);
}

void Editor::erase_pos_word () noexcept {
	acc.clear();
	if (at_end()) [[unlikely]] return;

	const size_t pos_save = pos;
	move_next_word();
	buf.erase (pos_save, pos - pos_save);
	pos = pos_save;
}

void Editor::erase_prev_word () noexcept {
	acc.clear();
	if (at_begin()) [[unlikely]] return;

	const size_t pos_save = pos;
	move_prev_word();
	buf.erase (pos, pos_save - pos);
}

void Editor::erase_begin_to_pos () noexcept {
	acc.clear();
	if (at_begin()) [[unlikely]] return;

	buf.erase (0, pos);
	pos = 0;
}

void Editor::erase_pos_to_end () noexcept {
	acc.clear();
	if (at_end()) [[unlikely]] return;

	buf.erase (pos);
}

}
