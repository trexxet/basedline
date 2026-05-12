#pragma once

#include <string>
#include <string_view>

#include "Basedlib/Container/StaticVector.hpp"

namespace Basedline {

class Editor {
	std::u8string buf;
	size_t pos = 0;
	Basedlib::StaticVector <char, 4> acc;

	const char8_t* chbuf_at_u8 (size_t i) const noexcept { return buf.data() + i; }
	const char8_t* chbuf_u8 () const noexcept { return chbuf_at_u8 (0); }
	const char8_t* chbuf_pos_u8 () const noexcept { return chbuf_at_u8 (pos); }

	const char* chbuf_at (size_t i) const noexcept { return reinterpret_cast <const char*> (chbuf_at_u8 (i)); }
	const char* chbuf () const noexcept { return chbuf_at (0); }
	const char* chbuf_pos () const noexcept { return chbuf_at (pos); }

public:
	bool at_begin () const noexcept { return pos == 0; }
	bool at_end () const noexcept { return pos == buf.size(); }
	bool empty () const noexcept { return buf.empty(); }
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

	std::string_view   get()        const noexcept { return { chbuf(), buf.size() }; };
	std::string_view   get_pos()    const noexcept { return { chbuf_pos(), buf.size() - pos }; };
	std::u8string_view get_u8()     const noexcept { return buf; };
	std::u8string_view get_u8_pos() const noexcept { return { chbuf_pos_u8(), buf.size() - pos }; };
};

}
