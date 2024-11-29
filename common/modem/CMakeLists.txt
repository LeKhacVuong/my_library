cmake_minimum_required(VERSION 3.22)

project(modem)

set(CMAKE_C_STANDARD 99)

add_library(modem STATIC
        v_modem.h
        v_modem.c)

target_link_libraries(modem c_utils)
