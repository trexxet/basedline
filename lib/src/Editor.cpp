#include "Basedline/Editor.hpp"

extern "C" {
#include <grapheme.h>
}

namespace Basedline {

void Editor::clear () noexcept {
	acc.clear();
	_buf.clear();
	_pos = 0;
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
	_buf.insert (_pos, str);
	_pos += str.size();
}

void Editor::move_next_grapheme () noexcept {
	acc.clear();
	if (at_end()) [[unlikely]] return;
	_pos += grapheme_next_character_break_utf8 (chbuf_tail(), SIZE_MAX);
}

// TODO: maybe try optimizing for large strings
void Editor::move_prev_grapheme () noexcept {
	acc.clear();
	if (at_begin()) [[unlikely]] return;
	size_t i = 0;
	do {
		size_t new_pos_next = i + grapheme_next_character_break_utf8 (chbuf_at (i), SIZE_MAX);
		if (new_pos_next == _pos) {
			_pos = i;
			break;
		}
		i = new_pos_next;
	} while (i < _pos);
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
		_pos += grapheme_next_word_break_utf8 (chbuf_tail(), SIZE_MAX);
	} while (!at_end() && move_word_skip (*chbuf_tail_u8()));
}

// TODO: maybe try optimizing for large strings
void Editor::move_prev_word () noexcept {
	acc.clear();
	if (at_begin()) [[unlikely]] return;
	do {
		size_t i = 0;
		do {
			size_t new_pos_next = i + grapheme_next_word_break_utf8 (chbuf_at (i), SIZE_MAX);
			if (new_pos_next >= _pos) {
				_pos = i;
				break;
			}
			i = new_pos_next;
		} while (i < _pos);
	} while (!at_begin() && move_word_skip (*chbuf_tail_u8()));
}

void Editor::move_begin () noexcept {
	acc.clear();
	_pos = 0;
}

void Editor::move_end () noexcept {
	acc.clear();
	_pos = _buf.size();
}

void Editor::erase_pos_grapheme () noexcept {
	acc.clear();
	if (at_end()) [[unlikely]] return;

	const size_t pos_save = _pos;
	move_next_grapheme();
	_buf.erase (pos_save, _pos - pos_save);
	_pos = pos_save;
}

void Editor::erase_prev_grapheme () noexcept {
	acc.clear();
	if (at_begin()) [[unlikely]] return;

	const size_t pos_save = _pos;
	move_prev_grapheme();
	_buf.erase (_pos, pos_save - _pos);
}

void Editor::erase_pos_word () noexcept {
	acc.clear();
	if (at_end()) [[unlikely]] return;

	const size_t pos_save = _pos;
	move_next_word();
	_buf.erase (pos_save, _pos - pos_save);
	_pos = pos_save;
}

void Editor::erase_prev_word () noexcept {
	acc.clear();
	if (at_begin()) [[unlikely]] return;

	const size_t pos_save = _pos;
	move_prev_word();
	_buf.erase (_pos, pos_save - _pos);
}

void Editor::erase_begin_to_pos () noexcept {
	acc.clear();
	if (at_begin()) [[unlikely]] return;

	_buf.erase (0, _pos);
	_pos = 0;
}

void Editor::erase_pos_to_end () noexcept {
	acc.clear();
	if (at_end()) [[unlikely]] return;

	_buf.erase (_pos);
}

}
