cmake_minimum_required(VERSION 3.24)
project(Selex_Bootloader C)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_C_STANDARD 99)

include_directories(./
        app/
        app/board/
        app/canopen/
        app/canopen/od/
        app/canopen/init/
        firmware/
        host/
        utils/
        libs/canopen/
        boot_interface/)

add_executable(Selex_Bootloader main.c
        host/sm_host.h
        host/sm_host.c
        host/sm_cmd.h
        firmware/sm_segment.h
        firmware/sm_fw.h
        firmware/sm_segment.c
        firmware/sm_fw.c
        utils/CRC.c
        utils/sm_memory.h
        utils/sm_logger.h
        utils/sm_logger.c
        utils/sm_err.h
        utils/sm_timer.h
        libs/canopen/CO.c
        libs/canopen/CO_CAN_Msg.c
        libs/canopen/CO_CRC.c
        libs/canopen/CO_EMCY.c
        libs/canopen/CO_FLASH.c
        libs/canopen/CO_NMT.c
        libs/canopen/CO_OD.c
        libs/canopen/CO_OD_sign.c
        libs/canopen/CO_OD_storage.c
        libs/canopen/CO_Object.c
        libs/canopen/CO_PDO.c
        libs/canopen/CO_RPDO.c
        libs/canopen/CO_SDO.c
        libs/canopen/CO_SDOclient.c
        libs/canopen/CO_SDOserver.c
        libs/canopen/CO_SYNC.c
        libs/canopen/CO_TPDO.c

        libs/canopen/CO.h
        libs/canopen/CO_CAN_Msg.h
        libs/canopen/CO_CRC.h
        libs/canopen/CO_EMCY.h
        libs/canopen/CO_FLASH.h
        libs/canopen/CO_NMT.h
        libs/canopen/CO_OD.h
        libs/canopen/CO_OD_sign.h
        libs/canopen/CO_OD_storage.h
        libs/canopen/CO_Object.h
        libs/canopen/CO_PDO.h
        libs/canopen/CO_RPDO.h
        libs/canopen/CO_SDO.h
        libs/canopen/CO_SDOclient.h
        libs/canopen/CO_SDOserver.h
        libs/canopen/CO_SYNC.h
        libs/canopen/CO_TPDO.h
        libs/canopen/CO_config.h
        libs/canopen/CO_slave.h
        libs/canopen/CO_utils.h

        boot_interface/sm_boot_if.h
        boot_interface/sm_boot_host.c
        boot_interface/sm_boot_host.h
        boot_interface/sm_boot_canopen.h
        boot_interface/sm_boot_canopen.c
        boot_interface/sm_boot_spi_flash.h
        boot_interface/sm_boot_spi_flash.c
        app/sm_bootloader.h
        app/sm_bootloader.c
        app/board/desktop/sm_desktop_board.c
        app/board/desktop/sm_desktop_board.h
        app/canopen/init/app_co_init.c
        app/canopen/od/od.c
        app/canopen/od/comm_od/comm_od.c
        app/canopen/od/comm_od/comm_od_default_value.c
        app/canopen/od/manu_od/manu_od.c
        app/canopen/od/manu_od/manu_od_default_value.c
        app/board/desktop/sm_desktop_file_storage.c
        app/board/desktop/sm_desktop_spi_flash_simulation.c utils/linux/tcp_server.h utils/linux/tcp_server.c app/board/desktop/sm_desktop_canopen_if.h app/board/desktop/sm_desktop_canopen_if.c)
