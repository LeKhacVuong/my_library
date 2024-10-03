cmake_minimum_required(VERSION 3.22)
project(tiny-json)

set(CMAKE_C_STANDARD 99)

add_library(tiny-json STATIC
        tiny-json.c
        tiny-json.h)

