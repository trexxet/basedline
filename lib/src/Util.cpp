#include "Util.hpp"

#include "Basedlib/Text/Widechar.hpp"

#include "wrap/ncurses.h"

namespace Basedline::Util {

int bl_wcwidth (wchar_t wch) {
	if (wch == L'\b') return -1;
	if (wch < 32 || (wch >= 0x7f && wch < 0xa0)) return 0;
	if (Basedlib::Text::is_high_surrogate (wch)) return 2;
	if (Basedlib::Text::is_low_surrogate (wch)) return 0;

	int wchwidth = wcwidth (wch);
	if (wchwidth < 0) wchwidth = 2;
	return wchwidth;
}

}
