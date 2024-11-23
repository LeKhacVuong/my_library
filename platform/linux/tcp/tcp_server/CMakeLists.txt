cmake_minimum_required(VERSION 3.22)

project(tcp_server)

set(CMAKE_C_STANDARD 99)

add_library(tcp_server STATIC
        tcp_server.c
        tcp_server.h)
