#pragma once

#include <string>

#include "Basedlib/Class.hpp"
#include "Basedlib/RingBuffer.hpp"
#include "wrap/ncurses.h"

#include "Defs.hpp"

namespace Basedline {

struct Pad {
	struct Lines {
		using LinesRb = Basedlib::RingBuffer<std::wstring>;
		LinesRb lines;

		LinesRb& operator() () { return lines; }

		Lines () = delete;
		Lines (size_t bufLines) : lines (bufLines) { }
		BASED_CLASS_NO_COPY_MOVE (Lines);
	} lines;

	inline const std::wstring& operator[] (size_t i) const { return lines.lines[i]; }

	WINDOW* ncpad;

	conarea_t area;
	int scrollOffset = 0;

	void draw () {
		pnoutrefresh (ncpad, 0, 0, area.y1, area.x1, area.y2, area.x2);
	};

	inline int height () const { return area.y2 - area.y1 + 1; }
	inline int last () const { return lines.lines.size() - scrollOffset - 1; }
	inline int first () const { return last() >= height() ? last() - height() + 1 : 0; }

	inline void scrollUp () { if (first() > 0) scrollOffset++; }
	inline void scrollDown () { if (scrollOffset > 0) scrollOffset--; }

	Pad () = delete;
	Pad (consize_t size, size_t bufLines) : lines (bufLines) {
		ncpad = newpad (size.y, size.x);
	}
	~Pad () { delwin (ncpad); }

	class Iterator {
		const Pad& p;
		size_t idx;
	public:
		Iterator (const Pad& p, size_t idx) : p (p), idx (idx) { }
		bool operator!= (const Iterator& other) const { return idx != other.idx; }
		const std::wstring& operator*() const { return p[idx]; }
		Iterator& operator++() { idx++; return *this; }
	};

	Iterator begin() const { return Iterator (*this, first()); }
	Iterator end() const { return Iterator (*this, last() + 1); }

	BASED_CLASS_NO_COPY_MOVE (Pad);
};

}
