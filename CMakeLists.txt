cmake_minimum_required(VERSION 3.16)

cmake_policy(SET CMP0077 NEW)

# MSVC include all standard libs - no dependencies
cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# statically link exe against runtime C/C++ libs for linux
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")  # gcc
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libstdc++ -static-libgcc")
endif()

# set the project name
project(mineflow_project
        DESCRIPTION "mineflow"
        LANGUAGES C CXX)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# add dependencies libs
add_subdirectory(lib)

# ######################################################
# MINEFLOW LIBRARY - STATIC
# ######################################################

# add the library
set(SOURCE_FILES
    src/mineflow.cpp
)

add_library(mineflow STATIC ${SOURCE_FILES})

# library properties (not applied to external libraries)
set_target_properties(
	mineflow PROPERTIES
    PREFIX ""
    OUTPUT_NAME "mineflow"
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	C_STANDARD 11
	C_STANDARD_REQUIRED ON
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# compiler warnings level
if(MSVC)
  target_compile_options(mineflow PRIVATE /W4)
else()
  target_compile_options(mineflow PRIVATE -Wall)
endif()

# add includes
target_include_directories(mineflow PUBLIC 
	include
)

# ######################################################
# UNIT TESTS - CATCH 2
# ######################################################

set(PSEUDOFLOW_TESTS
	#tests/newman1/test_pseudoflow_newman1.cpp
	#tests/zuck_small/test_pseudoflow_zuck_small.cpp
	tests/kd/test_pseudoflow_kd.cpp
	#tests/zuck_medium/test_pseudoflow_zuck_medium.cpp
	#tests/p4hd/test_pseudoflow_p4hd.cpp
	#tests/marvin/test_pseudoflow_marvin.cpp
	#tests/w23/test_pseudoflow_w23.cpp
	#tests/zuck_large/test_pseudoflow_zuck_large.cpp
	#tests/sm2/test_pseudoflow_sm2.cpp
    #tests/mclaughlin_limit/test_pseudoflow_mclaughlin_limit.cpp
    tests/mclaughlin/test_pseudoflow_mclaughlin.cpp
)

# Create a target for executing the Catch unit tests.
add_executable(unitTests
	tests/testClock.c
	${PSEUDOFLOW_TESTS}
)

# add Catch2 libraries
target_link_libraries(unitTests
    Catch2::Catch2WithMain
	mineflow
)

set_target_properties(unitTests PROPERTIES
	OUTPUT_NAME "unitTests"
	SUFFIX ".exe"
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	C_STANDARD 11
	C_STANDARD_REQUIRED ON
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin/tests"
	LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin/tests"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin/tests"
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/tests"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/tests"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/tests"
)

target_include_directories(unitTests PUBLIC 
	include
	tests
)
