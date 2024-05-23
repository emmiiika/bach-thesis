# add_custom_target(
#     implementation.h
#     COMMAND {}configure_file(${CMAKE_CURRENT_SOURCE_DIR}/implementation_basic.h ${CMAKE_CURRENT_SOURCE_DIR}/implementation.h COPYONLY)
# )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/implementation_basic.h
               ${CMAKE_CURRENT_SOURCE_DIR}/implementation.h
               COPYONLY)



set(TEST_SOURCES1 01vertices_edges_halfedges.cpp)
add_executable(impl_tests1 ${TEST_SOURCES1})
target_include_directories(impl_tests1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(impl_tests1 gtest gtest_main)
target_link_libraries(impl_tests1 bagraph)
target_link_libraries(impl_tests1 stdc++)

set(TEST_SOURCES2 02creating_deleting_graph.cpp)
add_executable(impl_tests2 ${TEST_SOURCES2})
target_include_directories(impl_tests2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(impl_tests2 gtest gtest_main)
target_link_libraries(impl_tests2 bagraph)
target_link_libraries(impl_tests2 stdc++)

set(TEST_SOURCES3 03basic_graph_traversals.cpp)
add_executable(impl_tests3 ${TEST_SOURCES3})
target_include_directories(impl_tests3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(impl_tests3 gtest gtest_main)
target_link_libraries(impl_tests3 bagraph)
target_link_libraries(impl_tests3 stdc++)

set(TEST_SOURCES4 04searching.cpp)
add_executable(impl_tests4 ${TEST_SOURCES4})
target_include_directories(impl_tests4 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(impl_tests4 gtest gtest_main)
target_link_libraries(impl_tests4 bagraph)
target_link_libraries(impl_tests4 stdc++)

set(TEST_SOURCES5 05graph_constructing_functions.cpp)
add_executable(impl_tests5 ${TEST_SOURCES5})
target_include_directories(impl_tests5 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(impl_tests5 gtest gtest_main)
target_link_libraries(impl_tests5 bagraph)
target_link_libraries(impl_tests5 stdc++)

set(TEST_SOURCES6 06mappings.cpp)
add_executable(impl_tests6 ${TEST_SOURCES6})
target_include_directories(impl_tests6 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(impl_tests6 gtest gtest_main)
target_link_libraries(impl_tests6 bagraph)
target_link_libraries(impl_tests6 stdc++)

set(TEST_SOURCES7 07renumbering.cpp)
add_executable(impl_tests7 ${TEST_SOURCES7})
target_include_directories(impl_tests7 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(impl_tests7 gtest gtest_main)
target_link_libraries(impl_tests7 bagraph)
target_link_libraries(impl_tests7 stdc++)


set(TEST_ALL all.cpp)
add_executable(impl_tests_all ${TEST_ALL})
target_include_directories(impl_tests_all PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(impl_tests_all PRIVATE -ftest-coverage)
target_link_libraries(impl_tests_all gtest gtest_main)
target_link_libraries(impl_tests_all bagraph)
target_link_libraries(impl_tests_all stdc++)



#gtest_add_tests(TARGET impl_tests1 TEST_PREFIX old:)
#3gtest_discover_tests(impl_tests1 TEST_PREFIX new:)
#gtest_add_tests(TARGET impl_tests2 TEST_PREFIX old:)
#$gtest_discover_tests(impl_tests2 TEST_PREFIX new:)
#gtest_add_tests(TARGET impl_tests3 TEST_PREFIX old:)
#gtest_discover_tests(impl_tests3 TEST_PREFIX new:)
# set_target_properties(test01 PROPERTIES
# CXX_STANDARD 17
# CXX_EXTENSIONS OFF)
# add_dependencies(test01 implementation.h)
