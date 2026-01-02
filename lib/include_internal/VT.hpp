#pragma once

#define ESC "\x1b"
#define CSI ESC"["

#define VT_CUF CSI"%dC"
#define VT_CUB CSI"%dD"
#define VT_CUP CSI"%d;%dH"
#define VT_DL  CSI"%dM"

#define VT_DECSC ESC"7"
#define VT_DECSR ESC"8"
