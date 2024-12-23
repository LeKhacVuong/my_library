//
// Created by vuonglk on 29/11/2024.
//

#include "sm_crc.h"

#include <unistd.h>
#include <string.h>
#include "sim_a7680c.h"
#include "sm_logger.h"
#include "elapsed_timer.h"
#include "v_timer.h"
#include "v_unix_serial.h"

#define TAG "gps"

#define TEST_HOST   "test.mosquitto.org"
#define TEST_PORT   1883


char* g_git_link = "https://raw.githubusercontent.com/LeKhacVuong/http_file_storage/refs/heads/master/test_file.txt";


void my_timer_1ms_callback(void* arg){

}

int main(){

    sm_logger_init(0, LOG_LEVEL_INFO);
    LOG_INF(TAG, "Start app test gps");

    v_timer_t* timer = v_timer_create(1000, my_timer_1ms_callback, 0);

    v_serial_t* serial = unix_serial_create(UNIX_COM_0, 115200, UNIX_SERIAL_MODE_NONE_BLOCKING);


    if(!serial){
        LOG_ERR(TAG, "Open port FAILED");
        return -1;
    }

    sim_a7680c_create_default(serial);
    sleep(1);

    sim_a7680c_soft_reset();

    for(int i = 0; i < 20; i++){
        LOG_INF(TAG, "Wait sim reset done after %d", 20 - i);
        sleep(1);
    }

    LOG_INF(TAG, "Done reset module sim");

    sim_a786c_start_http_mode();
    sleep(5);
    sim_a786c_http_download_full_content(g_git_link);
    sleep(5);
    sim_a786c_http_load_content(0, 50, NULL);
    while (1){

    }
}