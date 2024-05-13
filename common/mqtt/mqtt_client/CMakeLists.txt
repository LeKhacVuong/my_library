cmake_minimum_required(VERSION 3.22)
project(mqtt_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

add_library(mqtt_client STATIC
        sm_mqtt_client.c
        sm_mqtt_client.h)

target_link_libraries(mqtt_client mqttc)