#include "VtCommand.hpp"

#include <cstdio>

#define ESC "\x1b"
#define CSI ESC"["

#define VT_DECSC ESC"7"
#define VT_DECSR ESC"8"

#define VT_CUH CSI"H"
#define VT_CUP CSI"%d;%dH"
#define VT_DECTCEM_L CSI"?25l"
#define VT_DECTCEM_H CSI"?25h"
#define VT_DL  CSI"%dM"
#define VT_ED2 CSI"2J"

namespace Basedline::VtCommand {

void clear () {
	std::printf (VT_CUH VT_ED2);
}

void clear_lines (tsize_t from, tsize_t count) {
	if (count <= 0) [[unlikely]] return;
	if (from < 0) [[unlikely]] from = 0;
	std::printf (VT_DECSC VT_CUP VT_DL VT_DECSR, from + 1, 1, count);
}

void cursor_to (tpos_t pos) {
	std::printf (VT_CUP, pos.y + 1, pos.x + 1);
}

void cursor_hide () {
	std::printf (VT_DECTCEM_L);
}

void cursor_show () {
	std::printf (VT_DECTCEM_H);
}

}
