add_library(basedline STATIC)

include(cmake/getBasedlib.cmake)

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
	Basedline.cpp
	ConsoleHandle.cpp
	Input.cpp
	LineEdit.cpp
	ReadState.cpp
)
list(TRANSFORM BASEDLINE_SOURCES PREPEND src/)

target_sources(basedline PRIVATE ${BASEDLINE_SOURCES})
target_include_directories(basedline PUBLIC include/)
target_include_directories(basedline PRIVATE include_internal/)
target_link_libraries(basedline PUBLIC basedlib)
