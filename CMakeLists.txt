cmake_minimum_required(VERSION 3.22)

project(ev-sdk)

set(CMAKE_C_STANDARD 99)

include_directories(utils
        utils/algorithms
        utils/timing
        utils/unix
        utils/containers)

add_subdirectory(utils)