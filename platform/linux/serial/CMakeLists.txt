cmake_minimum_required(VERSION 3.22)

project(unix_serial)

set(CMAKE_C_STANDARD 99)

add_library(unix_serial STATIC
        v_unix_serial.c
        v_unix_serial.h)
