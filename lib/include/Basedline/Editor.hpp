#pragma once

#include <string>
#include <string_view>

#include "Basedlib/Container/StaticVector.hpp"

namespace Basedline {

class Editor {
	std::u8string _buf;
	size_t _pos = 0; // byte offset
	Basedlib::StaticVector <char, 4> acc;

	const char8_t* chbuf_at_u8 (size_t i) const noexcept { return _buf.data() + i; }
	const char8_t* chbuf_u8 () const noexcept { return chbuf_at_u8 (0); }
	const char8_t* chbuf_tail_u8 () const noexcept { return chbuf_at_u8 (_pos); }

	const char* chbuf_at (size_t i) const noexcept { return reinterpret_cast <const char*> (chbuf_at_u8 (i)); }
	const char* chbuf () const noexcept { return chbuf_at (0); }
	const char* chbuf_tail () const noexcept { return chbuf_at (_pos); }

public:
	size_t pos () const noexcept { return _pos; }
	bool at_begin () const noexcept { return _pos == 0; }
	bool at_end () const noexcept { return _pos == _buf.size(); }

	size_t size () const noexcept { return _buf.size(); }
	bool empty () const noexcept { return _buf.empty(); }
	bool acc_empty () const noexcept { return acc.empty(); }

	void clear () noexcept;

	void accumulate (char8_t byte);
	void insert (std::u8string_view u8str);

	void move_next_grapheme () noexcept;
	void move_prev_grapheme () noexcept;
	void move_next_word () noexcept;
	void move_prev_word () noexcept;
	void move_begin () noexcept;
	void move_end () noexcept;

	void erase_pos_grapheme () noexcept;
	void erase_prev_grapheme () noexcept;
	void erase_pos_word () noexcept;
	void erase_prev_word () noexcept;
	void erase_begin_to_pos () noexcept;
	void erase_pos_to_end () noexcept;

	std::string_view   buf ()        const noexcept { return { chbuf(), _buf.size() }; };
	std::string_view   buf_tail ()   const noexcept { return { chbuf_tail(), _buf.size() - _pos }; };
	std::u8string_view u8buf ()      const noexcept { return _buf; };
	std::u8string_view u8buf_tail () const noexcept { return { chbuf_tail_u8(), _buf.size() - _pos }; };
};

}
