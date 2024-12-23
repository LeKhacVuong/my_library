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

bool get_signal(){
    return get_tick_count()%2;
}

void read_signal_period_callback(uint32_t _period){
    ///TODO lam gi thi lam
}

uint32_t signal_counter = 0;

void my_timer_1us_callback(void* arg){    // ngat timer 1 us (us la micro giay nhe!!)
    bool signal = get_signal();
    if(signal){
        signal_counter++;

    } else{
        if(signal_counter == 0){
            return;
        }
        read_signal_period_callback(signal_counter); // chu ky tinh theo us
        signal_counter = 0;
    }
}



void my_timer_1ms_callback(void* arg){

}

void my_msg_coming_cb(MQTT_CLIENT_ID id,
                      const char* topic,
                      const char* payload,
                      uint32_t payload_len,
                      void* arg){
    LOG_INF(TAG, "User recv msg - client id: %d, topic: %s, payload: %s", id, topic, payload);
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

    char buffer[512] = {0, };

    sim_a7868c_mqtt_callback_t mqtt_cb = {
            .msg_coming_cb = my_msg_coming_cb,
            .arg = NULL
    };

    sim_a7860c_reg_msg_coming_callback(mqtt_cb);


    sim_a7680c_soft_reset();

    for(int i = 0; i < 20; i++){
        LOG_INF(TAG, "Wait sim reset done after %d", 20 - i);
        sleep(1);
    }

    LOG_INF(TAG, "Done reset module sim");

    sim_a7860c_start_mqtt_mode();

    sleep(1);

    sim_a7860c_mqtt_init_client(MQTT_CLIENT_0, "vuonglk", MQTT_SERVER_TYPE_TCP);

    sleep(1);

    sim_a7860c_mqtt_set_will_msg(MQTT_CLIENT_0, "vuonglk123", "is error disconnected", 0);


    mqtt_client_info_t info = {
            .host = "test.mosquitto.org",
            .port = 1883,
            .keep_alive = 600,
            .clean_section = true
    };

    sim_a7860c_mqtt_connect(MQTT_CLIENT_0, info);

    sleep(3);

    sim_a7860c_mqtt_subscribe(MQTT_CLIENT_0, "hello_vuong", 0);

    while (1){
        sim_a7860c_mqtt_polling();
    }
}