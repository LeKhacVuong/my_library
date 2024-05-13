//
// Created by vuonglk on 13/05/2024.
//

#ifndef SM_SV_MQTT_CLIENT_H
#define SM_SV_MQTT_CLIENT_H

#include "stdint.h"

typedef void (*publish_response_callback)(void** state, struct mqtt_response_publish *publish);

typedef struct sm_mqtt_client_connection_info{
    char* broker_addr;
    char* port;
    char* client_id;
    char* user_name;
    char* user_password;
}sm_mqtt_client_connection_info_t;

typedef void sm_mqtt_client_t;

sm_mqtt_client_t* sm_mqtt_client_create(sm_mqtt_client_connection_info_t _info, publish_response_callback _cb);

int32_t sm_mqtt_client_subscribe(sm_mqtt_client_t* _this, const char* _topic);

int32_t sm_mqtt_client_unsubcribe(sm_mqtt_client_t* _this, const char* _topic);

int32_t sm_mqtt_client_publish(sm_mqtt_client_t* _this, const char* _topic, const char* _data, uint32_t _len);

int32_t sm_mqtt_client_disconnect(sm_mqtt_client_t* _this);

void sm_mqtt_client_process(sm_mqtt_client_t* _this);

int32_t sm_mqtt_client_destroy(sm_mqtt_client_t* _this);

#endif //SM_SV_MQTT_CLIENT_H
