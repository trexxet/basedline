#pragma once

#include <string>

namespace Basedline {

struct Debug {
	static FILE* f;
#if defined(BASEDLINE_DEBUG)
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
