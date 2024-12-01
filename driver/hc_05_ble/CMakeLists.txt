cmake_minimum_required(VERSION 3.22)

project(hc_05)

set(CMAKE_C_STANDARD 99)

add_library(hc_05 STATIC
        hc_05_base.h
        hc_05_base.c)

target_link_libraries(hc_05 modem)

add_executable(hc_05_test example.c)

target_link_libraries(hc_05_test hc_05 unix_timer unix_serial)

