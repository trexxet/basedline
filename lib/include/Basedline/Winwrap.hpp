#pragma once

#ifdef __WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
// bruh
#undef STRICT
#undef DELETE
#endif
