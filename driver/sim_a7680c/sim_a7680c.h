//
// Created by vuonglk on 28/11/2024.
//

#ifndef LIBS_STORAGE_GPS_GY_NEOS_H
#define LIBS_STORAGE_GPS_GY_NEOS_H
#include "stdint.h"
#include "v_serial.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t sim_a7680c_init_driver(v_serial_t* _serial);

int32_t sim_a7680c_process();

int32_t sim_a7680c_check_connect();

#ifdef __cplusplus
}
#endif

#endif //LIBS_STORAGE_GPS_GY_NEOS_H
