#include "Output.hpp"

#include "Basedlib/Text/Multibyte.hpp"

#include "Console.hpp"
#include "Debug.hpp"
#include "Util.hpp"

namespace Basedline {

void OutputHandler::slice_to_lines (const std::wstring& wcs) {
	struct {
		size_t start;
		int width;
	} slice = {0};

	auto make_slice = [&] (size_t where, bool newline = false) {
		pad.lines().push (wcs.substr (slice.start, where - slice.start));
		slice.start = newline ? where + 1 : where;
		slice.width = 0;
	};

	for (size_t i = 0; i < wcs.size(); i++) {
		if (wcs[i] == L'\n') {
			make_slice (i, true);
			continue;
		}
		int cw = Util::bl_wcwidth (wcs[i]);
		if (cw > 0 && slice.width + cw > con.size.x)
			make_slice (i);
		slice.width += cw;
	}
	if (slice.start < wcs.size())
		make_slice (wcs.size());
}

void OutputHandler::print (std::string_view s) {
	std::wstring wcs = Basedlib::Text::mbs_to_wcs (s);
	slice_to_lines (wcs);
	messages.push (std::move (wcs));
	pad.scrollOffset = 0;
}

void OutputHandler::sync () {
	if (pad.lines().size() == 0) [[unlikely]] return;
	if (lock) return;
	werase (pad.ncpad);

	int y = 0;
	for (const auto& str : pad) {
		mvwaddwstr (pad.ncpad, y, 0, str.c_str());
		y++;
	}

	pad.draw();
}

// TODO: different size for messages?
OutputHandler::OutputHandler (Console& con, size_t bufLines) : con (con), pad (con.size, bufLines), messages (bufLines) {
	pad.area = {0, 0, con.size.y - con.acLineY - 1, con.size.x - 1};
}

}
