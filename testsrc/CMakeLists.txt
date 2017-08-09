add_executable(unit_tests OBJECT
  unit_tests.cpp
)

target_include_directories(unit_tests
	PUBLIC
		${PROJECT_SOURCE_DIR}/lib/googletest/googletest/include
		${PROJECT_SOURCE_DIR}/lib/googletest/googlemock/include
)

target_include_directories(unit_tests
	PRIVATE
		${PROJECT_SOURCE_DIR}/testsrc
		${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(unit_tests gtest gtest_main)

add_test(
	NAME
		unit_tests
	COMMAND
		unit_tests
	WORKING_DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}
)
