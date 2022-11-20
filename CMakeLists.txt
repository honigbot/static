cmake_minimum_required(VERSION 3.19)
project(static)

add_library(static INTERFACE)
target_include_directories(static INTERFACE .)
target_compile_options(static INTERFACE -Wall -Wextra -pedantic)

add_executable(log_default tests/log_default.c)
target_link_libraries(log_default PUBLIC static)

add_executable(log_custom tests/log_custom.c)
target_link_libraries(log_custom PUBLIC static)