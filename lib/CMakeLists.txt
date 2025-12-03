add_library(basedline STATIC)

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

target_sources(basedline PRIVATE
	src/Basedline.cpp
	src/ConsoleHandle.cpp
	src/Cursor.cpp
	src/TTY.cpp
	$<$<CONFIG:Debug>:src/Debug.cpp>
)
target_include_directories(basedline PUBLIC include/)
