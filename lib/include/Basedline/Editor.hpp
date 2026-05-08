#pragma once

#include <string>
#include <string_view>

#include "Basedlib/Container/StaticVector.hpp"

namespace Basedline {

class Editor {
	std::u8string buf;
	size_t pos = 0;
	Basedlib::StaticVector <char, 4> acc; 

	const char* chbuf_at (size_t i) const noexcept {
		return reinterpret_cast <const char*> (buf.data() + i);
	}
	const char* chbuf () const noexcept { return chbuf_at (0); }
	const char* chbuf_pos () const noexcept { return chbuf_at (pos); }

public:
	bool at_begin () const noexcept { return pos == 0; }
	bool at_end () const noexcept { return pos == buf.size(); }
	bool empty () const noexcept { return buf.empty(); }

	void accumulate (char8_t byte);
	void insert (std::u8string_view u8str);

	void move_next_grapheme () noexcept;
	void move_prev_grapheme () noexcept;
	void move_begin () noexcept;
	void move_end () noexcept;

	std::string_view   get()        const noexcept { return { chbuf(), buf.size() }; };
	std::string_view   get_pos()    const noexcept { return { chbuf_pos(), buf.size() - pos }; };
	std::u8string_view get_u8()     const noexcept { return buf; };
	std::u8string_view get_u8_pos() const noexcept { return { buf.data() + pos, buf.size() - pos }; };
};

}
