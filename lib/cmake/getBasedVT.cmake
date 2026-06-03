include(FetchContent)

option(USE_LOCAL_BASEDVT "Use local BasedVT instance instead of FetchContent" OFF)

if (NOT TARGET basedvt)
	if (USE_LOCAL_BASEDVT)
		set(BASEDVT_PATH ${PROJECT_SOURCE_DIR}/../basedvt)
		message(STATUS "Using local BasedVT from ${BASEDVT_PATH}")
		FetchContent_Declare(
			basedvt
			SOURCE_DIR ${BASEDVT_PATH}
		)
	else()
		message(STATUS "Using BasedVT from git")
		FetchContent_Declare(
			basedvt
			GIT_REPOSITORY https://gitlab.com/based1337/basedlib.git
			GIT_TAG        main
			GIT_SHALLOW    TRUE
		)
	endif()
	FetchContent_MakeAvailable(basedvt)
endif()
