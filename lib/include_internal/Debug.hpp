#pragma once

#include "Basedlib/LogFile.hpp"

namespace Basedline {

inline Basedlib::LogFile& fdbg_inst() {
	static Basedlib::LogFile dbg (
		#if defined(BASEDLINE_DEBUG)
			"bl_debug.txt"
		#else
			nullptr
		#endif
	);
	return dbg;
}

}

#if defined(BASEDLINE_DEBUG)
# include <format>
# define BL_DEBUG(...) Basedline::fdbg_inst().print (std::format (__VA_ARGS__))
#else  // BASEDLINE_DEBUG
# define BL_DEBUG(...)
#endif // BASEDLINE_DEBUG
