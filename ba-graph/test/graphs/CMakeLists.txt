# add_custom_target(
#     implementation.h
#     COMMAND {}configure_file(${CMAKE_CURRENT_SOURCE_DIR}/implementation_basic.h ${CMAKE_CURRENT_SOURCE_DIR}/implementation.h COPYONLY)
# )

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/implementation_basic.h
#               ${CMAKE_CURRENT_SOURCE_DIR}/implementation.h
#               COPYONLY)



set(TEST_SOURCES test_cpg.cpp main.cpp)

add_executable(graphs_tests ${TEST_SOURCES})
#target_include_directories(test01 PRIVATE ../../include)
#target_include_directories(graphs_tests PRIVATE ${GTEST_INCLUDE_DIRS})
target_include_directories(graphs_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(graphs_tests gtest) #gtest_main)
target_link_libraries(graphs_tests bagraph)
target_link_libraries(graphs_tests stdc++)


gtest_add_tests(TARGET graphs_tests TEST_PREFIX old:)
gtest_discover_tests(graphs_tests TEST_PREFIX new:)
# set_target_properties(test01 PROPERTIES
# CXX_STANDARD 17
# CXX_EXTENSIONS OFF)
# add_dependencies(test01 implementation.h)
