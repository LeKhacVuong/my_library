cmake_minimum_required(VERSION 3.22)

project(libs-storage)

set(CMAKE_C_STANDARD 99)

include_directories(utils
        utils/algorithms
        utils/timing
        utils/unix
        utils/containers
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
        )

add_subdirectory(utils)
add_subdirectory(libs/nanoModbus)
add_subdirectory(libs/host)
add_subdirectory(libs/MQTT-C)
add_subdirectory(common/modbus/mb_master)
add_subdirectory(common/modbus/mb_slave)
add_subdirectory(common/mqtt/mqtt_client)
add_subdirectory(common/http)
