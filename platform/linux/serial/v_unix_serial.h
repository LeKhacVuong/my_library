//
// Created by vuonglk on 29/11/2024.
//

#ifndef LIBS_STORAGE_UNIX_SERIAL_IMPL_H
#define LIBS_STORAGE_UNIX_SERIAL_IMPL_H
#include "v_serial.h"

typedef enum{
    UNIX_SERIAL_MODE_BLOCKING,
    UNIX_SERIAL_MODE_UNBLOCKING
}UNIX_SERIAL_MODE;

v_serial_t* unix_serial_create(const char* _port, uint32_t _baud, UNIX_SERIAL_MODE _mode) __wur;

#endif //LIBS_STORAGE_UNIX_SERIAL_IMPL_H
