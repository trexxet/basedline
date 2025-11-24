#pragma once

#include <string>

namespace Basedline::Debug {
#if defined(BASEDLINE_DEBUG)
void open ();
void print (const std::string& str);
void close ();
#else
inline void open () {}
inline void print (const std::string& str) {}
inline void close () {}
#endif
}
