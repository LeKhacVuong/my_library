cmake_minimum_required(VERSION 3.5)
project(MQTT-C VERSION 1.1.2 LANGUAGES C)

# MQTT-C build options

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# MQTT-C library
add_library(mqttc STATIC
    src/mqtt_pal.c
    src/mqtt.c
)
target_include_directories(mqttc PUBLIC include)
target_link_libraries(mqttc PUBLIC 
    $<$<C_COMPILER_ID:MSVC>:ws2_32>
)

# Handle multi-lib linux systems correctly and allow custom installation locations.
if(UNIX)
	include(GNUInstallDirs)
	mark_as_advanced(CLEAR
		CMAKE_INSTALL_BINDIR
		CMAKE_INSTALL_LIBDIR
		CMAKE_INSTALL_INCLUDEDIR)
else()
	set(CMAKE_INSTALL_LIBDIR "lib")
	set(CMAKE_INSTALL_INCLUDEDIR "include")
endif()

# Install includes and library
install(TARGETS mqttc 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
