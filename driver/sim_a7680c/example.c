//
// Created by vuonglk on 29/11/2024.
//

#include <unistd.h>
#include "sim_a7680c.h"
#include "sm_logger.h"
#include "elapsed_timer.h"
#include "v_timer.h"
#include "v_unix_serial.h"

#define TAG "gps"

void my_timer_callback(void* arg){
//    LOG_INF(TAG, "Timer callback");
}

int main(){
    sm_logger_init(0, LOG_LEVEL_INFO);
    LOG_INF(TAG, "Start app test gps");

    v_timer_t* timer = v_timer_create(1000, my_timer_callback, 0);

    v_serial_t* serial = unix_serial_create(UNIX_COM_0, 9600, UNIX_SERIAL_MODE_NONE_BLOCKING);


    if(!serial){
        LOG_ERR(TAG, "Open port FAILED");
        return -1;
    }

    sim_a7680c_init_driver(serial);

    while (1){
        sim_a7680c_check_connect();
        sleep(2);
    }
}