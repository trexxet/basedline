#pragma once

#include <string>

#if defined(BASEDLINE_DEBUG)
#include <format>
# define BL_DEBUG(...) Debug::print (std::format (__VA_ARGS__))
#else
# define BL_DEBUG(...)
#endif

namespace Basedline {

struct Debug {
#if defined(BASEDLINE_DEBUG)
	static FILE* f;
	Debug ();
	static void print (const std::string& str);
	~Debug ();
#else
	inline Debug () {}
	inline void print (const std::string& str) {}
	inline ~Debug () {}
#endif
};

}
