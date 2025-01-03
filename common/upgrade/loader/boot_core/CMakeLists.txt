cmake_minimum_required(VERSION 3.22)
project(boot-core)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

add_library(boot_core SHARED
            sm_bootloader.c
            sm_bootloader.h
            fw/sm_segment.h
            fw/sm_fw.h
            fw/sm_segment.c
            fw/sm_fw.c)

target_link_libraries(boot_core boot_if boot_utils)