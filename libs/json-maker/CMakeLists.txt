cmake_minimum_required(VERSION 3.22)
project(json-maker)

set(CMAKE_C_STANDARD 99)

add_library(json-maker STATIC
        json-maker .c
        json-maker .h)

