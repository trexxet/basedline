#pragma once

#if defined(_WIN32)
#include <windows.h>
#endif

namespace Basedline {

#if defined(_WIN32)
using termsize_t = SHORT;
using coord_t = COORD;
#endif

}
