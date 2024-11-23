cmake_minimum_required(VERSION 3.22)

project(tcp_client)

set(CMAKE_C_STANDARD 99)

add_library(tcp_client STATIC
            tcp_client.c
            tcp_client.h)
