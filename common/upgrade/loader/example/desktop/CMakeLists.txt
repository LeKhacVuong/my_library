cmake_minimum_required(VERSION 3.22)
project(boot-simulator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

add_executable(boot_simulator
        main.c
        sm_desktop_board.c
        sm_desktop_board.h
        sm_desktop_file_storage.c
        sm_desktop_spi_flash_simulation.c
        sm_desktop_canopen_if.h
        sm_desktop_canopen_if.c)

target_link_libraries(boot_simulator boot_core boot_utils)