include(GoogleTest)

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

set(INSTALL_GTEST OFF)
set(INSTALL_GMOCK OFF)

add_subdirectory(../thirdparty/googletest ../thirdparty/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


enable_testing()
add_subdirectory(impl)
add_subdirectory(graphs)