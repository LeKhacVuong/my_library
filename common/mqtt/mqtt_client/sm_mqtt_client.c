//
// Created by vuonglk on 13/05/2024.
//

#include "sm_mqtt_client.h"

#include <mqtt.h>
#include <stdlib.h>
#include "mqtt.h"
#include "posix_sockets.h"
#include "sm_logger.h"

#define TAG "mqtt_client"

#define MQTT_MAX_SIZE_BUFFER (3*1024)
#define MQTT_QOS_USE MQTT_PUBLISH_QOS_0

typedef struct sm_mqtt_client_impl {
    sm_mqtt_client_connection_info_t m_connection_info;
    publish_response_callback m_publish_response_callback;
    struct mqtt_client m_mqtt_instance;
    uint8_t m_sendBuffer[MQTT_MAX_SIZE_BUFFER];
    uint8_t m_rcvBuffer[MQTT_MAX_SIZE_BUFFER];
} sm_mqtt_client_impl_t;

#define impl(x) ((sm_mqtt_client_impl_t*)(x))


sm_mqtt_client_t* sm_mqtt_client_create(sm_mqtt_client_connection_info_t _info, publish_response_callback _cb) {
    sm_mqtt_client_impl_t* this = malloc(sizeof(sm_mqtt_client_impl_t));
    if (this == NULL) return NULL;

    int sockfd = open_nb_socket(_info.broker_addr, _info.port);
    if (sockfd < 0) {
        LOG_ERR(TAG, "Failed to open socket");
        free(this);
        return NULL;
    }

    mqtt_init(&this->m_mqtt_instance, sockfd, this->m_sendBuffer, MQTT_MAX_SIZE_BUFFER, this->m_rcvBuffer,
              MQTT_MAX_SIZE_BUFFER, _cb);

    mqtt_connect(&this->m_mqtt_instance, _info.client_id, NULL, NULL, 0,
                 _info.user_name, _info.user_password, MQTT_CONNECT_CLEAN_SESSION, 400);

    if (this->m_mqtt_instance.error != MQTT_OK) {
        LOG_ERR(TAG, "Connect to MQTT Broker failed with ERROR code %s", mqtt_error_str(this->m_mqtt_instance.error));
        free(this);
        return NULL;
    }

    LOG_INF(TAG, "Create Mqtt Client and connected to broker address %s SUCCESS", _info.broker_addr);

    this->m_connection_info = _info;
    this->m_publish_response_callback = _cb;

    return this;
}

int32_t sm_mqtt_client_subscribe(sm_mqtt_client_t* _this, const char* _topic) {
    sm_mqtt_client_impl_t* this = impl(_this);
    if(this == NULL) return -1;

    mqtt_subscribe(&this->m_mqtt_instance, _topic, MQTT_QOS_USE);
    if (this->m_mqtt_instance.error != MQTT_OK) {
        LOG_ERR(TAG, "Subscribe to Topic %s FAILED with ERROR code %s", _topic,  mqtt_error_str(this->m_mqtt_instance.error));
        return -1;
    }
    LOG_INF(TAG, "Subscribe to Topic %s SUCCESS", _topic);
    return 0;
}

int32_t sm_mqtt_client_unsubcribe(sm_mqtt_client_t* _this, const char* _topic) {
    sm_mqtt_client_impl_t* this = impl(_this);
    if(this == NULL) return -1;

    mqtt_unsubscribe(&this->m_mqtt_instance, _topic);
    if (this->m_mqtt_instance.error != MQTT_OK) {
        LOG_ERR(TAG, "Subscribe to Topic %s FAILED with ERROR code %s", _topic,  mqtt_error_str(this->m_mqtt_instance.error));
        return -1;
    }
    LOG_INF(TAG, "Subscribe to Topic %s SUCCESS", _topic);
    return 0;
}

int32_t sm_mqtt_client_publish(sm_mqtt_client_t* _this, const char* _topic, const char* _data, uint32_t _len) {
    sm_mqtt_client_impl_t* this = impl(_this);
    if(this == NULL) return -1;
    mqtt_publish(&this->m_mqtt_instance, _topic, _data, _len, MQTT_QOS_USE);
    if (this->m_mqtt_instance.error != MQTT_OK) {
        LOG_ERR(TAG, "Publish message to Topic %s failed with ERROR code %s", _topic,  mqtt_error_str(this->m_mqtt_instance.error));
        return -1;
    }
    return 0;
}

int32_t sm_mqtt_client_disconnect(sm_mqtt_client_t* _this) {
    sm_mqtt_client_impl_t* this = impl(_this);
    if(this == NULL) return -1;

    mqtt_disconnect(&this->m_mqtt_instance);
    LOG_INF(TAG, "Disconnected to Mqtt Broker");
    return 0;
}

void sm_mqtt_client_process(sm_mqtt_client_t* _this) {
    sm_mqtt_client_impl_t* this = impl(_this);
    if(this == NULL) return;
    mqtt_sync(&this->m_mqtt_instance);
}

int32_t sm_mqtt_client_destroy(sm_mqtt_client_t* _this) {
    if(_this == NULL) return -1;
    free(_this);
    return 0;
}