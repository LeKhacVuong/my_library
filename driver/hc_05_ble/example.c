//
// Created by vuonglk on 29/11/2024.
//

#include <unistd.h>
#include <string.h>
#include "hc_05_base.h"
#include "sm_logger.h"
#include "elapsed_timer.h"
#include "v_timer.h"
#include "v_unix_serial.h"

#define TAG "hc_05_test"

void my_timer_1ms_callback(void* arg){
//    LOG_INF(TAG, "Timer callback");
}

int main(){
    sm_logger_init(0, LOG_LEVEL_INFO);
    LOG_INF(TAG, "Start app test gps");

    v_timer_t* timer = v_timer_create(1000, my_timer_1ms_callback, 0);

    v_serial_t* serial = unix_serial_create(UNIX_COM_0, 9600, UNIX_SERIAL_MODE_NONE_BLOCKING);

    hc_05_proc_t* hc_proc = hc_05_create(serial, NULL, NULL);

    if(!serial){
        LOG_ERR(TAG, "Open port FAILED");
        return -1;
    }

    char name[32] = {0,};

    while (1){
        hc_proc->set_pass(hc_proc, "Selex123");
        sleep(3);
        hc_proc->get_pass(hc_proc, name, 32);
        sleep(3);
    }
}