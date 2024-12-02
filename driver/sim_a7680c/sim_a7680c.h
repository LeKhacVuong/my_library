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

#define TPC_ID_MAX 9 // 0 - 9

typedef enum {
    TCP_RX_MANUAL,
    TCP_RX_AUTO,
    TCP_RX_MODE_NUMBER
}TCP_RX_MODE;

/*************************************** GENERAL COMMANDS ************************************/

int32_t sim_a7680c_init_driver(v_serial_t* _serial);

int32_t sim_a7680c_check_connect();

int32_t sim_a7680c_soft_reset();

int32_t sim_a7680c_check_sim_slot();

int32_t sim_a7860c_open_network();

int32_t sim_a7860c_close_network();

int32_t sim_a7860c_get_net_work_register_status(char* _buffer, uint32_t _max_len);

int32_t sim_a7860c_get_net_work_quality(char* _buffer, uint32_t _max_len);

int32_t sim_a7860c_get_ip(char* _buffer, uint32_t _max_len);

int32_t sim_a7680_get_imei(char* _buffer, uint32_t _max_len);

int32_t sim_a7680c_process();


/*************************************** TCP COMMANDS ************************************/

int32_t sim_a7680_tcp_set_rx_mode(TCP_RX_MODE _mode);

int32_t sim_a7680_tcp_open(uint8_t _tcp_link_id, const char* _host, uint32_t _port);

int32_t sim_a7680_tcp_close(uint8_t _tcp_link_id);


/*************************************** MQTT COMMANDS ************************************/

int32_t sim_a7860c_start_mqtt_mode();

int32_t sim_a7860c_stop_mqtt_mode();


#ifdef __cplusplus
}
#endif

#endif //LIBS_STORAGE_GPS_GY_NEOS_H
