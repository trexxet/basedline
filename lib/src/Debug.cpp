#if defined(BASEDLINE_DEBUG)
#include "Debug.hpp"

#include <cstdio>
#include <stdexcept>

#define BASEDLINE_DEBUG_FILENAME "bl_debug.txt"

namespace Basedline {

FILE* Debug::f = nullptr;

Debug::Debug () {
	f = fopen (BASEDLINE_DEBUG_FILENAME, "w");
	if (!f) [[unlikely]]
		throw std::runtime_error ("Can't open " BASEDLINE_DEBUG_FILENAME " for write");
}

void Debug::print (const std::string& str) {
	if (f) {
		std::fprintf (f, str.c_str());
		fflush(f);
	}
}

Debug::~Debug () {
	if (f) {
		fclose (f);
		f = nullptr;
	}
}

}
#endif
