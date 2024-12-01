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

typedef union datetime{
    uint8_t arr_datetime[6];
    struct {
        uint8_t year;
        uint8_t mon;
        uint8_t day;
        uint8_t hour;
        uint8_t min;
        uint8_t sec;
    };
}gps_date_time_t;

int32_t gps_gy_create_driver(v_serial_t* _serial);

int32_t gps_gy_process();

#ifdef __cplusplus
}
#endif

#endif //LIBS_STORAGE_GPS_GY_NEOS_H
