add_custom_target(basedline_test_all)

function(add_basedline_test testName)
	add_executable(${testName} EXCLUDE_FROM_ALL ${ARGN})
	target_link_libraries(${testName} PRIVATE basedline)
	add_dependencies(basedline_test_all ${testName})
	add_test(NAME ${testName} COMMAND ${testName})
endfunction()

add_basedline_test(bl_test_editor testEditor.cpp)
