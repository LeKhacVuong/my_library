cmake_minimum_required(VERSION 3.22)

project(libs-storage)

set(CMAKE_C_STANDARD 99)

include_directories(utils
        utils/code_c/algorithms
        utils/code_c/timing
        utils/code_c/string
        utils/code_c/unix
        utils/code_c/containers
        utils/code_c

        libs
        libs/nanoModbus
        libs/host
        libs/MQTT-C

        common
        common/modbus
        common/modbus/mb_slave
        common/modbus/mb_master
        common/mqtt
        common/mqtt/mqtt_client
        common/http
        common/modem

        platform/linux/tcp/tcp_client
        platform/linux/tcp/tcp_server
        )

add_subdirectory(utils/code_c)
add_subdirectory(libs/nanoModbus)
add_subdirectory(libs/host)
add_subdirectory(libs/MQTT-C)
add_subdirectory(common/modbus/mb_master)
add_subdirectory(common/modbus/mb_slave)
add_subdirectory(common/mqtt/mqtt_client)
add_subdirectory(common/http)
add_subdirectory(common/modem)
add_subdirectory(common/modules)
add_subdirectory(platform/linux/tcp/tcp_client)
add_subdirectory(platform/linux/tcp/tcp_server)
