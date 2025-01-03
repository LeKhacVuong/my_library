cmake_minimum_required(VERSION 3.22)
project(boot-interface)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

add_library(boot_if STATIC
        sm_boot_if.h
        sm_boot_host.c
        sm_boot_host.h
        sm_boot_canopen.h
        sm_boot_canopen.c
        canopen/init/app_co_init.c
        canopen/od/od.c
        canopen/od/comm_od/comm_od.c
        canopen/od/comm_od/comm_od_default_value.c
        canopen/od/manu_od/manu_od.c
        canopen/od/manu_od/manu_od_default_value.c
        sm_boot_spi_flash.h
        sm_boot_spi_flash.c)

target_link_libraries(boot_if co host boot_utils)