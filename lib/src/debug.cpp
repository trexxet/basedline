#include "Debug.hpp"

#include <cstdio>
#include <stdexcept>

#define BASEDLINE_DEBUG_FILENAME "bl_debug.txt"

namespace Basedline::Debug {
#if defined(BASEDLINE_DEBUG)

FILE* f;

void open () {
	f = fopen (BASEDLINE_DEBUG_FILENAME, "w");
	if (!f)
		throw std::runtime_error ("Can't open " BASEDLINE_DEBUG_FILENAME " for write");
}

void print (const std::string& str) {
	if (f) std::fprintf(f, str.c_str());
}

void close () {
	if (f) fclose (f);
}

#endif
}
