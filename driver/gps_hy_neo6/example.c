//
// Created by vuonglk on 29/11/2024.
//

#include <unistd.h>
#include "gps_gy_neo6.h"
#include "sm_logger.h"
#include "elapsed_timer.h"
#include "v_timer.h"

#define TAG "gps"

void my_timer_callback(void* arg){
    int* val = arg;

    LOG_INF(TAG, "Timer callback, value %d", *val);
}


int main(){
    sm_logger_init(0, LOG_LEVEL_INFO);
    LOG_INF(TAG, "Start app test gps");

    int val = 1999;

    v_timer_t* timer = v_timer_create(1000, my_timer_callback , &val);

    while (1){

    }
}