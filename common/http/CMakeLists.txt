cmake_minimum_required(VERSION 3.22)

project(http-client)

set(CMAKE_C_STANDARD 99)

add_library(http_client STATIC
        sm_http_client.h
        http_client_curl.c)

target_link_libraries(http_client -lcurl utils)

add_executable(http_client_exam_down_file
        example_download_file.c)

target_link_libraries(http_client_exam_down_file http_client)
