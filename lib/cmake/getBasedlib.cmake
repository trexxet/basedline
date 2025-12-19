include(FetchContent)

option(USE_LOCAL_BASEDLIB "Use local Basedlib instance instead of FetchContent" OFF)

if (NOT TARGET basedlib)
	if (USE_LOCAL_BASEDLIB)
		set(BASEDLIB_PATH ${PROJECT_SOURCE_DIR}/../basedlib)
		message(STATUS "Using local Basedlib from ${BASEDLIB_PATH}")
		FetchContent_Declare(
			basedlib
			SOURCE_DIR ${BASEDLIB_PATH}
		)
	else()
		message(STATUS "Using Basedlib from git")
		FetchContent_Declare(
			basedlib
			GIT_REPOSITORY https://gitlab.com/trexxet/basedlib.git
			GIT_TAG        main
			GIT_SHALLOW    TRUE
		)
	endif()
	FetchContent_MakeAvailable(basedlib)
endif()
