include(ExternalProject)

if(WIN32)
	set(LG_LIB_FILE libgrapheme.lib)
else()
	set(LG_LIB_FILE libgrapheme.a)
endif()

ExternalProject_Add(
	libgrapheme_src
	GIT_REPOSITORY git://git.suckless.org/libgrapheme
	GIT_TAG 3.0.0
	UPDATE_DISCONNECTED TRUE
	CONFIGURE_COMMAND
		bash -lc "cd '<SOURCE_DIR>' && ./configure"
	BUILD_COMMAND
		bash -lc "cd '<SOURCE_DIR>' && MAKEFLAGS= make --silent"
	INSTALL_COMMAND
		bash -lc "cd '<SOURCE_DIR>' && MAKEFLAGS= make --silent install PREFIX='<INSTALL_DIR>'"
	BUILD_BYPRODUCTS <SOURCE_DIR>/${LG_LIB_FILE}
	INSTALL_BYPRODUCTS <INSTALL_DIR>/lib/${LG_LIB_FILE}
)

ExternalProject_Get_Property(libgrapheme_src INSTALL_DIR)

add_library(libgrapheme STATIC IMPORTED)
set_target_properties(libgrapheme PROPERTIES
	IMPORTED_LOCATION ${INSTALL_DIR}/lib/${LG_LIB_FILE}
	INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include
)
add_dependencies(libgrapheme libgrapheme_src)
