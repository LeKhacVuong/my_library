//
// Created by vuonglk on 29/11/2024.
//

#ifndef LIBS_STORAGE_V_SERIAL_H
#define LIBS_STORAGE_V_SERIAL_H
#include "stdint.h"

typedef enum{
    SERIAL_TX,
    SERIAL_RX,
    SERIAL_TX_RX
}SERIAL_DIRECTION;

typedef struct v_serial v_serial_t;

struct v_serial{
    int32_t     (*open)(v_serial_t*, uint32_t baud);
    int32_t     (*close)(v_serial_t*);
    int32_t     (*set_baud)(v_serial_t*, uint32_t baud);
    int32_t     (*is_open)(v_serial_t*);
    uint32_t    (*bytes_available)(v_serial_t*);
    int32_t     (*flush)(v_serial_t*, SERIAL_DIRECTION dir);
    int32_t     (*send)(v_serial_t*, char* data, uint32_t len);
    int32_t     (*read)(v_serial_t*, char* buff, uint32_t max_len);
    int32_t     (*read_blocking)(v_serial_t*, char* buff, uint32_t max_len, uint32_t timeout);
};

#endif //LIBS_STORAGE_V_SERIAL_H
