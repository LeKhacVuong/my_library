cmake_minimum_required(VERSION 3.16)
project(ev-sdk-utils)

set(CMAKE_C_STANDARD 99)

set(UTILS_SRC
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
        unix/serial/unix_serial.c
        unix/serial/unix_serial.h
)

add_library(utils STATIC
        ${UTILS_SRC})
