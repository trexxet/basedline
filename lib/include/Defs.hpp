#pragma once

#if defined(_WIN32)
#include <windows.h>
#endif

#define BASEDLINE_CLASS_COPY(c, d) \
	c (const c&) = d; \
	c& operator= (const c&) = d;
#define BASEDLINE_CLASS_MOVE(c, d) \
	c (c&&) = d; \
	c& operator= (c&&) = d;
#define BASEDLINE_CLASS_NO_COPY(c) BASEDLINE_CLASS_COPY(c, delete)
#define BASEDLINE_CLASS_NO_MOVE(c) BASEDLINE_CLASS_MOVE(c, delete)
#define BASEDLINE_CLASS_NO_COPY_MOVE(c) \
	BASEDLINE_CLASS_NO_COPY(c); \
	BASEDLINE_CLASS_NO_MOVE(c);

namespace Basedline {

#if defined(_WIN32)
using termsize_t = SHORT;
using coord_t = COORD;
#endif

}
