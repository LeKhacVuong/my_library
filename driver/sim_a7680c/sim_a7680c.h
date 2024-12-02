//
// Created by vuonglk on 28/11/2024.
//

#ifndef LIBS_STORAGE_GPS_GY_NEOS_H
#define LIBS_STORAGE_GPS_GY_NEOS_H

#include <stdbool.h>
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

typedef void v_sim_a76x_t;

/*************************************** GENERAL COMMANDS ************************************/

v_sim_a76x_t* sim_a7680c_create_default(v_serial_t* _serial);

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

typedef struct {
    char*       host;
    int         port;
    char*       user_name;
    char*       user_password;
    bool        clean_section;
    uint32_t    keep_alive;
}mqtt_client_info_t;

typedef enum {
    MQTT_CLIENT_0,
    MQTT_CLIENT_1,
    MQTT_CLIENT_ID_NUMBER
}MQTT_CLIENT_ID;

typedef enum {
    MQTT_SERVER_TYPE_TCP,
    MQTT_SERVER_TYPE_SSL,
    MQTT_SERVER_TYPE_NUMBER
}MQTT_SERVER_TYPE;

#define MQTT_CLIENT_ID_0
#define MQTT_CLIENT_ID_1

int32_t sim_a7860c_start_mqtt_mode();  //Step 1

int32_t sim_a7860c_stop_mqtt_mode();

int32_t sim_a7860c_init_mqtt_client(MQTT_CLIENT_ID _client_id,
                                    const char* _client_name,
                                    MQTT_SERVER_TYPE _server_type); // Support ID 0 or 1

int32_t sim_a7860c_set_will_msg(MQTT_CLIENT_ID _client_id,
                                const char* _topic,
                                const char* _msg,
                                uint8_t _qos);         // Set before connect

int32_t sim_a7860c_connect(MQTT_CLIENT_ID _client_id,
                           mqtt_client_info_t _info);

int32_t sim_a7860c_check_connection(MQTT_CLIENT_ID _client_id);


int32_t sim_a7860c_disconnect(MQTT_CLIENT_ID _client_id, uint32_t _timeout);

int32_t sim_a7860c_subscribe(MQTT_CLIENT_ID _client_id, const char* _topic, uint8_t _qos);


#ifdef __cplusplus
}
#endif

#endif //LIBS_STORAGE_GPS_GY_NEOS_H
