cmake_minimum_required(VERSION 3.22)

project(gps_gy)

set(CMAKE_C_STANDARD 99)

add_library(gps_gy STATIC
        gps_gy_neo6.h
        gps_gy_neo6.c)

target_link_libraries(gps_gy modem)

add_executable(gps_test example.c)

target_link_libraries(gps_test gps_gy unix_timer)

