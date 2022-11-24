cmake_minimum_required(VERSION 3.16)
project(static)

add_library(static INTERFACE)
target_include_directories(static INTERFACE .)
target_compile_options(static INTERFACE -Wall -Wextra -pedantic)

add_executable(test_log_default tests/log_default.c)
target_link_libraries(test_log_default PUBLIC static)

add_executable(test_log_custom tests/log_custom.c)
target_link_libraries(test_log_custom PUBLIC static)

add_executable(test_test tests/test_test.c)
target_link_libraries(test_test PUBLIC static)

add_executable(test_event tests/test_event.c)
target_link_libraries(test_event PUBLIC static)