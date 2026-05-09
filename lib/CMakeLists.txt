add_library(basedline STATIC)

include(cmake/getBasedlib.cmake)
include(cmake/getLibgrapheme.cmake)

target_link_options(basedline PRIVATE
	-Wl,--export-all-symbols
)
set_target_properties(basedline PROPERTIES
	VERSION 0.1
	DEBUG_POSTFIX "dbg"
)
target_compile_definitions(basedline PRIVATE
	$<$<CONFIG:Debug>:BASEDLINE_DEBUG>
)

set(BASEDLINE_SOURCES
	Editor.cpp
)
list(TRANSFORM BASEDLINE_SOURCES PREPEND src/)

target_sources(basedline PRIVATE ${BASEDLINE_SOURCES})
target_include_directories(basedline PUBLIC include/)
target_link_libraries(basedline
	PUBLIC basedlib
	PRIVATE libgrapheme
)
