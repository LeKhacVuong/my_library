//
// Created by vuonglk on 29/11/2024.
//

#include <unistd.h>
#include <string.h>
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

    v_serial_t* serial = unix_serial_create(UNIX_COM_0, 115200, UNIX_SERIAL_MODE_NONE_BLOCKING);

    if(!serial){
        LOG_ERR(TAG, "Open port FAILED");
        return -1;
    }

    sim_a7680c_init_driver(serial);    sleep(1);


    char buffer[512] = {0, };


    sleep(1);

    sim_a7680_tcp_set_rx_mode(TCP_RX_AUTO);

    sleep(1);

    sim_a7860c_close_network();

    sleep(1);

    sim_a7860c_open_network();

    sleep(1);

    sim_a7680_tcp_open(1, "test.mosquitto.org", 1883);

    sleep(1);

    sim_a7860c_get_ip(buffer, 512);

    while (1){

        sleep(5);
        memset(buffer, 0, 512);
    }
}