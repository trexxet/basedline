add_custom_target(basedline_demo_all)

function(add_basedline_demo demoName)
	add_executable(${demoName} EXCLUDE_FROM_ALL ${ARGN})
	target_link_libraries(${demoName} basedline)
	add_dependencies(basedline_demo_all ${demoName})
endfunction()

add_basedline_demo(basedline_demo demo.cpp)
