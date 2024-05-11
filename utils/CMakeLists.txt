cmake_minimum_required(VERSION 3.16)
project(ev-sdk-utils)

set(CMAKE_C_STANDARD 99)

set(SDK_UTILS_SRC
        sm_logger.c
        sm_logger.h
        algorithms/sm_crc.c
        algorithms/sm_crc.h
        containers/sm_vector.c
        containers/sm_vector.h
        containers/sm_queue.c
        containers/sm_queue.h
        timing/elapsed_timer.h
        timing/TimeUtils.h
        timing/Timer.h
        unix/linux_serial.c
        unix/linux_serial.h
)

add_library(sdk_utils STATIC
        ${SDK_UTILS_SRC})
