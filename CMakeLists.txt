cmake_minimum_required(VERSION 3.16)
project(static)

add_library(static INTERFACE)
target_include_directories(static INTERFACE .)

add_executable(test_log_default tests/log_default.c)
target_link_libraries(test_log_default PUBLIC static)
target_compile_options(test_log_default PUBLIC -std=c11 -Wall -Wextra -pedantic)

add_executable(test_log_custom tests/log_custom.c)
target_link_libraries(test_log_custom PUBLIC static)
target_compile_options(test_log_custom PUBLIC -std=c11 -Wall -Wextra -pedantic)

add_executable(test_test tests/test_test.c)
target_link_libraries(test_test PUBLIC static)
target_compile_options(test_test PUBLIC -std=c11 -Wall -Wextra -pedantic)

