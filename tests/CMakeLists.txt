find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

set(TEST_FILES 
    ngx_url_parser_test.cpp
    main.cpp)

add_executable(unit_tests ${TEST_FILES})

target_link_libraries(unit_tests ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(unit_tests ${GTEST_BOTH_LIBRARIES})
target_link_libraries(unit_tests ngx_url_parser)

enable_testing()
add_test(A unit_tests)
