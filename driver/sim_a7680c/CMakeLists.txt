cmake_minimum_required(VERSION 3.22)

project(sim_a7680c)

set(CMAKE_C_STANDARD 99)

add_library(sim_a7680c STATIC
        sim_a7680c.h
        sim_a7680c.c)

target_link_libraries(sim_a7680c modem)

add_executable(test_sim mqtt_example.c)
target_link_libraries(test_sim sim_a7680c unix_timer unix_serial)


add_executable(test_http http_example.c)
target_link_libraries(test_http sim_a7680c unix_timer unix_serial)
