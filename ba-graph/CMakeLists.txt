cmake_minimum_required(VERSION 3.10)

project(ba-graph)
enable_language(CXX)

if (NOT CMAKE_BUILD_TYPE)
    message("CMAKE_BUILD_TYPE not specified! Setting to Debug.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

option(BUILD_TESTS "Build unit tests" ON)
option(WERROR "Add -Werror flag to build (turns warnings into errors)" ON)

#find_package(Boost REQUIRED)
#find_package(Threads)

# Set version number (change as needed). These definitions are available
# by including "exampleConfig.h" in the source.
# See exampleConfig.h.in for some more details.
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)


# Include stuff. No change needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")



# --------------------------------------------------------------------------------
#                          Compiler (change as needed).
# --------------------------------------------------------------------------------
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-fconcepts SUPPORT_CONCEPTS)
set(CMAKE_CXX_FLAGS "-fconcepts ${CMAKE_CXX_FLAGS}")

if(NOT SUPPORT_CONCEPTS)
    message(FATAL_ERROR "Your compiler does not support concepts; try to use >=gcc-6")
endif()

# Set the C++ standard you wish to use (will apply to all files).
# If you do not use any features that limits the standard required,
# you could omit this line.
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Things to always include as flags. Change as needed.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")

# Build-type specific flags. Change as needed.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# configure optimization # TODO: not finished yet
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OPTIMIZATION_FLAGS "-O0 -DDEBUG -DBA_GRAPH_DEBUG -D_GLIBCXX_DEBUG")
    # message("--- Configuring DEBUG build")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(OPTIMIZATION_FLAGS "-O3 -DNDEBUG")
    set(WERROR OFF)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(OPTIMIZATION_FLAGS "-Os -DNDEBUG")
    set(WERROR OFF)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(OPTIMIZATION_FLAGS "-O2 -DDEBUG")
else()
    message(FATAL_ERROR "Configuring '${CMAKE_BUILD_TYPE}' build, which is not supported")
endif()

# Enable extra warnings to adhere to https://github.com/mapbox/cpp/issues/37
#set(DESIRED_WARNINGS "-Wall -Wextra -Wconversion -Wunreachable-code -Wuninitialized -pedantic-errors -Wold-style-cast -Wno-error=unused-variable -Wshadow -Wfloat-equal -Weffc++")
set(DESIRED_WARNINGS "-pedantic -Wall -Wextra")
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(DESIRED_WARNINGS "${DESIRED_WARNINGS} -Wmost")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZATION_FLAGS} ${DESIRED_WARNINGS}")

if (WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()
message(STATUS "WERROR is ${WERROR}")
# Verbosely show additional cxxflags
message(STATUS "Building with the following extra flags: ${CMAKE_CXX_FLAGS}")

# --------------------------------------------------------------------------------
#                         Locate files
# --------------------------------------------------------------------------------
# We make sure that CMake sees all the files.
include_directories(
     ${PROJECT_SOURCE_DIR}/include
#     ${PROJECT_SOURCE_DIR}/thirdparty/googletest
#     ${PROJECT_SOURCE_DIR}/thirdparty/googletest/include
)

# --------------------------------------------------------------------------------
#                         The library
# --------------------------------------------------------------------------------

add_library(bagraph INTERFACE)
target_include_directories(bagraph INTERFACE "${PROJECT_SOURCE_DIR}/include/")

# --------------------------------------------------------------------------------
#                         Make Tests (TODO).
# --------------------------------------------------------------------------------
# Add a make target 'gtest', that runs the tests (and builds all dependencies).
# The setup of Google Test is done at the very end of this file.

if(${BUILD_TESTS})
    add_subdirectory(test)
endif()


# if(${BUILD_TESTS})
#     find_package(GTest)
#     find_package(Threads REQUIRED)
#     set(TEST_LIB googletest)
#     set(TEST_FILES   test/main.cpp
#         )
#     set(TEST_MAIN unit_tests.x)
# 	if (${GTEST_FOUND} AND NOT ${BUILD_OWN_GOOGLETEST})
#         include_directories(${GTEST_INCLUDE_DIRS})
#         set(TEST_LIB ${GTEST_BOTH_LIBRARIES})
#         add_executable(${TEST_MAIN} ${TEST_FILES})
# 	else()
# 		message("Google testing framework not found. Building our own!")
#         add_custom_target( git_update
#         COMMAND git submodule init
#         COMMAND git submodule update
#         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} )
#         add_library(googletest
#             ${PROJECT_SOURCE_DIR}/thirdparty/googletest/src/gtest-all.cc
#             ${PROJECT_SOURCE_DIR}/thirdparty/googletest/src/gtest_main.cc)
#         add_dependencies(googletest git_update)
#         set_source_files_properties(${PROJECT_SOURCE_DIR}/thirdparty/googletest/src/gtest-all.cc  PROPERTIES GENERATED 1)
#         set_source_files_properties(${PROJECT_SOURCE_DIR}/thirdparty/googletest/src/gtest_main.cc PROPERTIES GENERATED 1)
#         add_executable(${TEST_MAIN} ${TEST_FILES})
#         add_dependencies(${TEST_MAIN} ${TEST_LIB})
#     endif()

#     target_link_libraries(${TEST_MAIN} ${TEST_LIB} ${CMAKE_THREAD_LIBS_INIT})
#     add_custom_target(gtest COMMAND "${PROJECT_BINARY_DIR}/${TEST_MAIN}" DEPENDS ${TEST_MAIN})

#     # Add a standard make target 'test' that runs the tests under CTest (only as an alt. to gtest).
#     include(CTest)
#     enable_testing()
#     add_test(unit_tests ${PROJECT_BINARY_DIR}/${TEST_MAIN})
# endif()


# --------------------------------------------------------------------------------
#                            Build!
# --------------------------------------------------------------------------------



set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------------------------------------------------------------
#                            Install(TODO)!
# --------------------------------------------------------------------------------

## ??? TODO
