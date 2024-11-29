cmake_minimum_required(VERSION 3.22)

project(unix_timer)

set(CMAKE_C_STANDARD 99)

add_library(unix_timer STATIC
        v_timer.c
        v_timer.h)

target_link_libraries(unix_timer rt c_utils)