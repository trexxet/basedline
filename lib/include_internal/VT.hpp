#pragma once

#define ESC "\x1b"
#define CSI ESC"["

#define VT_CUU CSI"%dA"
#define VT_CUP CSI"%d;%dH"
#define VT_DL  CSI"%dM"
#define VT_SU  CSI"%dS"

#define VT_DECSC ESC"7"
#define VT_DECSR ESC"8"
