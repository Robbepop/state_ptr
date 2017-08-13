project(StatePointer)

cmake_minimum_required(VERSION 3.2.2)

#==============================================================================
# Setup for compiler toolchain.
#==============================================================================

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	set(COMPILING_WITH_CLANG true)
	set(COMPILING_WITH_GNULIKE true)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(COMPILING_WITH_GXX true)
	set(COMPILING_WITH_GNULIKE true)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(COMPILING_WITH_MSVC true)
	set(COMPILING_WITH_GNULIKE false)
else()
	message(WARNING "Unknown compiler ${CMAKE_CXX_COMPILER_ID}, compiling with default parameters.")
endif()

#==============================================================================
# Definitions for the various supported compilers.
#==============================================================================

if(COMPILING_WITH_GNULIKE)
	add_definitions(
		-std=c++14
		# -O0

		# -Wall
		# -Wextra
		# -Wpedantic
		# -Wconversion
		# -Wsign-conversion

		# -pedantic-errors
	)
endif()

if(COMPILING_WITH_CLANG)
	# Nothing to do, yet.
endif()

if(COMPILING_WITH_GCC)
	# Nothing to do, yet.
endif()

if(COMPILING_WITH_MSVC)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	add_definitions(/EHsc)
endif()

#==============================================================================
# Setup and enable testing.
#==============================================================================

add_subdirectory(lib/googletest)
enable_testing()
add_subdirectory(testsrc)
