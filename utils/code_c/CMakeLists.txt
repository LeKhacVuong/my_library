cmake_minimum_required(VERSION 3.16)
project(code_c_utils)

set(CMAKE_C_STANDARD 99)

set(C_UTILS_SRC
        sm_logger.c
        sm_logger.h

        algorithms/sm_crc.c
        algorithms/sm_crc.h
        algorithms/sm_intel_hex.h
        algorithms/sm_intel_hex.c

        containers/sm_vector.c
        containers/sm_vector.h
        containers/sm_queue.c
        containers/sm_queue.h
        containers/sm_stack.c
        containers/sm_stack.h

        timing/elapsed_timer.h
        timing/TimeUtils.h
        timing/Timer.h

        unix/serial/linux_serial.c
        unix/serial/linux_serial.h

        string/sm_string_t.c
        string/sm_string_t.h
        string/sm_string_util.c
        string/sm_string_util.h
)

add_library(c_utils STATIC
        ${C_UTILS_SRC})
