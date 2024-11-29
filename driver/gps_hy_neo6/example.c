//
// Created by vuonglk on 29/11/2024.
//

#include <unistd.h>
#include "gps_gy_neo6.h"
#include "sm_logger.h"
#include "elapsed_timer.h"
#include "v_timer.h"
#include "v_unix_serial.h"

#define TAG "gps"

void my_timer_callback(void* arg){
    LOG_INF(TAG, "Timer callback, value %d");

    v_serial_t* serial = arg;
    if(serial){
        serial->send(serial, "Vuong", 5);
    }
}


int main(){
    sm_logger_init(0, LOG_LEVEL_INFO);
    LOG_INF(TAG, "Start app test gps");

    v_serial_t* serial = unix_serial_create(UNIX_COM_0, 9600, UNIX_SERIAL_MODE_NONE_BLOCKING);

    v_timer_t* timer = v_timer_create(1000, my_timer_callback, serial);


    while (1){

    }
}