#pragma once

#include "Basedlib/FileDebug.hpp"

namespace Basedline {

inline Basedlib::FileDebug& fdbg_inst() {
	static Basedlib::FileDebug dbg (
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
