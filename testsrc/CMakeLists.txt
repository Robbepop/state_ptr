add_executable(unit_tests
  log2_tests.cpp
  state_ptr_tests.cpp
)

target_include_directories(unit_tests
	PUBLIC
		${PROJECT_SOURCE_DIR}/lib/googletest/googletest/include
		${PROJECT_SOURCE_DIR}/lib/googletest/googlemock/include
)

target_include_directories(unit_tests
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/testsrc
		${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(unit_tests gtest gtest_main)

if(COMPILING_WITH_GNULIKE)
	target_compile_options(unit_tests
		PUBLIC
			-O0             # disable optimizations as they screw testing
			-fno-inline -O0 # ^--- see above

			-Wall
			-Wextra
			-Wpedantic
			-Wconversion
			-Wsign-conversion

			-pedantic-errors # turns pedantic warnings into errors
	)
endif()

add_test(
	NAME
		unit_tests
	COMMAND
		unit_tests
	WORKING_DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}
)
