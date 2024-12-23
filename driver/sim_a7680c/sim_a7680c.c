//
// Created by lekhacvuong on 11/30/24.
//

#include <string.h>
#include <stdbool.h>
#include "sim_a7680c.h"
#include "v_modem.h"
#include "sm_logger.h"
#include "elapsed_timer.h"
#include "TimeUtils.h"

#include "v_math.h"

#define TAG "sim_a7680c"

#define LTE_BUFFER_LENGTH 2048
#define TIMEOUT_POLL_MSG  500

#define OK_RES     "OK\r\n"
#define ERROR_RES        "ERROR"

#define END_OF_LINE     "\r\n"

typedef struct{
    v_modem_t* m_modem;
    char m_buffer[LTE_BUFFER_LENGTH];

    struct{
        sim_a7868c_mqtt_callback_t m_cb;
        void* m_arg;
    }m_mqtt;
}sim_a7680c_driver_impl_t;

typedef enum {
    DATA_RAW,
    DATA_STRING,
    DATA_TYPE_NUMBER
}DATA_TYPE;


static sim_a7680c_driver_impl_t g_instance;

/*************************************** GENERAL COMMANDS ************************************/

static int32_t sim_a7860c_send_cmd(sim_a7680c_driver_impl_t* this, const char* _cmd, uint32_t _timeout){
    int32_t ret = -1;
    char buff[LTE_BUFFER_LENGTH] = {0,};

    if(_cmd){
        snprintf(buff, LTE_BUFFER_LENGTH, "AT+%s\r\n", _cmd);
    }else{
        snprintf(buff, LTE_BUFFER_LENGTH, "AT\r\n");
    }

    ret = modem_send_cmd(this->m_modem, buff, OK_RES, ERROR_RES, _timeout);
    if(ret < 0){
        LOG_ERR(TAG, "Set sim send CMD \"%s\" FAILED",_cmd);
        return -1;
    }

    LOG_INF(TAG, "Set sim send CMD \"%s\" SUCCEED",_cmd);
    return ret;
}

static int32_t sim_a7860c_set_value_res_ok(sim_a7680c_driver_impl_t* this,
                                           const char* _key,
                                           const char* _value,
                                           uint32_t _timeout){
    int32_t ret = -1;

    char buff[LTE_BUFFER_LENGTH] = {0,};

    snprintf(buff, LTE_BUFFER_LENGTH, "AT+%s=%s\r\n", _key, _value);

    ret = modem_send_cmd(this->m_modem, buff, OK_RES, ERROR_RES, _timeout);
    if(ret < 0){
        LOG_ERR(TAG, "Set sim key \"%s\" FAILED",_key);
        return -1;
    }

    LOG_INF(TAG, "Set sim key \"%s\" to %s SUCCEED",_key, _value);

    return ret;
}

static const char* sim_a7860c_set_value_res_echo(sim_a7680c_driver_impl_t* this,
                                               const char* _key,
                                               const char* _value,
                                               uint32_t _timeout){

    int32_t ret = -1;

    char buff[LTE_BUFFER_LENGTH] = {0,};

    snprintf(buff, LTE_BUFFER_LENGTH, "AT+%s=%s\r\n", _key, _value);

    ret = modem_send_cmd(this->m_modem, buff, _key, ERROR_RES, _timeout);
    if(ret < 0){
        LOG_ERR(TAG, "Set sim key \"%s\" FAILED",_key);
        return NULL;
    }
    LOG_INF(TAG, "Set sim key \"%s\" to %s SUCCEED", _key, _value);
    char* modem_buff = modem_get_buff(this->m_modem);
    char *p_res = strrchr(modem_buff, '+');
    return p_res;
}

static int32_t sim_a7860c_get_value(sim_a7680c_driver_impl_t* this,
                                    const char* _key,
                                    bool _qst_mrk,
                                    char* _buff,
                                    uint32_t _max_len,
                                    uint32_t _timeout){
    int32_t ret = -1;

    char buff[LTE_BUFFER_LENGTH] = {0,};

    snprintf(buff, LTE_BUFFER_LENGTH, "AT+%s%s\r\n", _key, _qst_mrk?"?":"");

    ret = modem_send_cmd(this->m_modem, buff, OK_RES, ERROR_RES, _timeout);
    if(ret < 0){
        LOG_ERR(TAG, "Get sim key \"%s\" FAILED", _key);
        return -1;
    }

    char value[LTE_BUFFER_LENGTH] = {0,};

    char format[128] = {0,};
    sprintf(format, "+%s: %s\r\n", _key, "%s");

    char* p_data = strrchr(modem_get_buff(this->m_modem), '+');  // Find last '+'

    ret = sscanf(p_data, format, value);
    LOG_INF(TAG, "Get sim key \"%s\" is: %s", _key, value);

    if(_buff){
        memcpy(_buff, value, v_min_off(_max_len, LTE_BUFFER_LENGTH));
    }

    return ret;
}

v_sim_a76x_t* sim_a7680c_create_default(v_serial_t* _serial){
    sim_a7680c_driver_impl_t* this = &g_instance;

    this->m_modem = modem_create_default(LTE_BUFFER_LENGTH, _serial);
    if(!this->m_modem){
        return NULL;
    }
    return this;
}

int32_t sim_a7680c_check_connect(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    return sim_a7860c_send_cmd(this, NULL, 1000);
}

int32_t sim_a7680c_soft_reset(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    return sim_a7860c_send_cmd(this, "CRESET", 1000);
}

int32_t sim_a7680c_check_sim_slot(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    return sim_a7860c_get_value(this, "CPIN", true, NULL, 0, 1000);
}

int32_t sim_a7860c_open_network(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    return sim_a7860c_send_cmd(this, "NETOPEN", 1000);
}

int32_t sim_a7860c_close_network(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    return sim_a7860c_send_cmd(this, "NETCLOSE", 1000);
}

int32_t sim_a7680_get_imei(char* _buffer, uint32_t _max_len){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    return sim_a7860c_get_value(this, "SIMEI", true, _buffer, _max_len, 1000);
}

int32_t sim_a7860c_get_net_work_register_status(char* _buffer, uint32_t _max_len){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    return sim_a7860c_get_value(this, "CREG", true, _buffer, _max_len, 1000);
}

int32_t sim_a7860c_get_net_work_quality(char* _buffer, uint32_t _max_len){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    return sim_a7860c_get_value(this, "CSQ", false, _buffer, _max_len, 1000);
}

int32_t sim_a7860c_get_ip(char* _buffer, uint32_t _max_len){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    return sim_a7860c_get_value(this, "IPADDR", false, _buffer, _max_len, 1000);
}

int32_t sim_a7680c_process(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    memset(this->m_buffer, 0, LTE_BUFFER_LENGTH);
    int len = modem_read_until_string(this->m_modem, END_OF_LINE, this->m_buffer, LTE_BUFFER_LENGTH, 5);  ///Add Events need
    if(len > 0){
        char* p_event = NULL;

    }

    return 0;
}

/*************************************** TCP COMMANDS ************************************/
int32_t sim_a7680_tcp_set_rx_mode(TCP_RX_MODE _mode){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    if(_mode == TCP_RX_AUTO){
        return sim_a7860c_set_value_res_ok(this, "CIPRXGET", "1", 1000);
    }else{
        return sim_a7860c_set_value_res_ok(this, "CIPRXGET", "0", 1000);
    }
}

int32_t sim_a7680_tcp_open(uint8_t _tcp_link_id, const char* _host, uint32_t _port){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    if(_tcp_link_id > TPC_ID_MAX){
        LOG_ERR(TAG, "Maximum tcp link id is %d", TPC_ID_MAX);
        return -1;
    }

    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "%d,\"TCP\",\"%s\",%d", _tcp_link_id, _host, _port);

    return sim_a7860c_set_value_res_ok(this, "CIPOPEN", value, 1000);
}

int32_t sim_a7680_tcp_close(uint8_t _tcp_link_id){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    if(_tcp_link_id > TPC_ID_MAX){
        LOG_ERR(TAG, "Maximum tcp link id is %d", TPC_ID_MAX);
        return -1;
    }

    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "%d", _tcp_link_id);
    return sim_a7860c_set_value_res_ok(this, "CIPCLOSE", value, 1000);
}

/*************************************** MQTT COMMANDS ************************************/

typedef enum{
    RES_TYPE_OK,
    RES_TYPE_ECHO,
    RES_TYPE_NUMBER
}MQTT_CMD_RES_TYPE;

static int32_t sim_a7860c_set_mqtt_value_echo_res(MQTT_CLIENT_ID _client_id,
                                                  const char* _key,
                                                  const char* _value,
                                                  uint32_t _timeout){
    sim_a7680c_driver_impl_t* this = &g_instance;

    int32_t ret = -1;
    char buff[LTE_BUFFER_LENGTH] = {0,};

    snprintf(buff, LTE_BUFFER_LENGTH, "AT+%s=%s\r\n", _key, _value);

    ret = modem_send_cmd(this->m_modem, buff, _key, ERROR_RES, _timeout);
    if(ret < 0){
        LOG_ERR(TAG, "Client %d set key \"%s\" FAILED by not echo",_client_id, _key);
        return -1;
    }

    char *p_log = strrchr(modem_get_buff(this->m_modem), '+');
    if(p_log){
        p_log  = strsep(&p_log, "\r\n");
    }
    LOG_INF(TAG, "Client %d set simply key \"%s\" to \"%s\" - echo: %s ",_client_id, _key, _value, p_log?p_log:"___");
    return 0;
}

static int32_t sim_a7860c_set_mqtt_value_with_fill_header(MQTT_CMD_RES_TYPE _res_type,
                                                          MQTT_CLIENT_ID _client_id,
                                                          const char* _key,
                                                          const char* _header,
                                                          const char* _value,
                                                          uint32_t _timeout){
    sim_a7680c_driver_impl_t* this = &g_instance;

    int ret = -1;

    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "AT+%s=%s\r\n", _key, _header);

    ret = modem_send_cmd(this->m_modem, value, ">\r\n", ERROR_RES, 500);
    if(ret < 0){
        LOG_ERR(TAG, "Client %d set header of key %s FAILED", _client_id, _key);
        return -1;
    }

    memset(value, 0, LTE_BUFFER_LENGTH);
    snprintf(value, LTE_BUFFER_LENGTH, "%s\r\n", _value);

    if(_res_type == RES_TYPE_OK){
        ret = modem_send_cmd(this->m_modem, value, OK_RES, ERROR_RES, _timeout);
        if(ret < 0){
            LOG_ERR(TAG, "Client %d set value of key %s FAILED by not OK", _client_id, _key);
            return -1;
        }
        LOG_INF(TAG, "Client %d set header key \"%s\" SUCCEED to \"%s\" ret OK",_client_id, _key, _value);
        return 0;
    }
    else {
        ret = modem_send_cmd(this->m_modem, value, _key, ERROR_RES, _timeout);
        if (ret < 0) {
            LOG_ERR(TAG, "Client %d set value of key %s FAILED by not echo", _client_id, _key);
            return -1;
        }
        char *p_log = strrchr(modem_get_buff(this->m_modem), '+');
        if(p_log){
            p_log  = strsep(&p_log, "\r\n");
        }
        LOG_INF(TAG, "Client %d set header key \"%s\" to \"%s\" - echo: %s ",_client_id, _key, _value, p_log?p_log:"___");
        return 0;
    }
}


int32_t sim_a7860c_start_mqtt_mode(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }
    return sim_a7860c_send_cmd(this, "CMQTTSTART", 1000);
}


int32_t sim_a7860c_stop_mqtt_mode(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }
    return sim_a7860c_send_cmd(this, "CMQTTSTOP", 1000);
}

int32_t sim_a7860c_reg_msg_coming_callback(sim_a7868c_mqtt_callback_t _cb){
    sim_a7680c_driver_impl_t* this = &g_instance;
    this->m_mqtt.m_cb = _cb;
}

int32_t sim_a7860c_mqtt_init_client(MQTT_CLIENT_ID _client_id, const char* _client_name, MQTT_SERVER_TYPE _server_type){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER || _server_type >= MQTT_SERVER_TYPE_NUMBER){
        return -1;
    }

    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "%d,\"%s\",%d", _client_id, _client_name, _server_type);

    return sim_a7860c_set_value_res_ok(this, "CMQTTACCQ", value, 1000);
}

int32_t sim_a7860c_mqtt_set_will_msg(MQTT_CLIENT_ID _client_id, const char* _topic, const char* _msg, uint8_t _qos){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER || _qos > 2){
        return -1;
    }

    int ret = -1;

    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "%d,%ld,%d", _client_id, strlen(_msg), _qos);

    ret = sim_a7860c_set_mqtt_value_with_fill_header(RES_TYPE_OK, _client_id,
                                                      "CMQTTWILLMSG", value, _msg, 500);
    if(ret < 0){
        LOG_ERR(TAG, "Set client %d will msg FAILED", _client_id);
        return ret;
    }

    memset(value, 0, LTE_BUFFER_LENGTH);
    snprintf(value, LTE_BUFFER_LENGTH, "%d,%ld", _client_id, strlen(_topic));
    return sim_a7860c_set_mqtt_value_with_fill_header(RES_TYPE_OK, _client_id,
                                                     "CMQTTWILLTOPIC", value, _topic, 500);
}

int32_t sim_a7860c_mqtt_connect(MQTT_CLIENT_ID _client_id,
                                mqtt_client_info_t _info){

    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER){
        return -1;
    }

    char value[LTE_BUFFER_LENGTH] = {0,};
    if(_info.user_name && _info.user_password){
        snprintf(value, LTE_BUFFER_LENGTH,
                 "%d,\"tcp://%s:%d\",%d,%d,\"%s\",\"%s\"",
                 _client_id,
                 _info.host,
                 _info.port,
                 _info.keep_alive,
                 _info.clean_section,
                 _info.user_name,
                 _info.user_password);
    }else{
        snprintf(value, LTE_BUFFER_LENGTH,
                 "%d,\"tcp://%s:%d\",%d,%d",
                 _client_id,
                 _info.host,
                 _info.port,
                 _info.keep_alive,
                 _info.clean_section);
    }

    return sim_a7860c_set_mqtt_value_echo_res(_client_id, "CMQTTCONNECT", value, 15000);
}

int32_t sim_a7860c_mqtt_check_connect(MQTT_CLIENT_ID _client_id){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER){
        return -1;
    }

    return 0;
}


int32_t sim_a7860c_mqtt_disconnect(MQTT_CLIENT_ID _client_id, uint32_t _timeout){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER){
        return -1;
    }

    char value[LTE_BUFFER_LENGTH] = {0,};
    if(_timeout){
        snprintf(value, LTE_BUFFER_LENGTH, "%d,%d", _client_id, _timeout);
    }else{
        snprintf(value, LTE_BUFFER_LENGTH, "%d", _client_id);
    }
    return sim_a7860c_set_mqtt_value_echo_res(_client_id, "CMQTTDISC", value, 5000);
}


int32_t sim_a7860c_mqtt_subscribe(MQTT_CLIENT_ID _client_id, const char* _topic, uint8_t _qos){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER || _qos > 2){
        return -1;
    }

    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "%d,%ld,%d", _client_id, strlen(_topic), _qos);

    return sim_a7860c_set_mqtt_value_with_fill_header(RES_TYPE_ECHO, _client_id,
                                                      "CMQTTSUB", value, _topic, 5000);
}

int32_t sim_a7860c_mqtt_unsubscribe(MQTT_CLIENT_ID _client_id, const char* _topic){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER){
        return -1;
    }

    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "%d,%ld", _client_id, strlen(_topic));


    sim_a7860c_set_mqtt_value_with_fill_header(RES_TYPE_OK, _client_id,
                                                      "CMQTTUNSUBTOPIC", value, _topic, 1000);

    return sim_a7860c_set_mqtt_value_echo_res(_client_id,
                                              "CMQTTUNSUB", value, 5000);
}


int32_t sim_a7860c_mqtt_publish(MQTT_CLIENT_ID _client_id,
                                const char* _topic,
                                const char* _payload,
                                uint8_t _qos,
                                uint32_t _timeout){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER || _qos > 2){
        return -1;
    }

    int ret = -1;

    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "%d,%ld", _client_id, strlen(_topic));

    ret = sim_a7860c_set_mqtt_value_with_fill_header(RES_TYPE_OK, _client_id,
                                                     "CMQTTTOPIC", value, _topic, 2000);
    if(ret < 0){
        LOG_ERR(TAG, "Set client %d topic FAILED", _client_id);
        return ret;
    }

    memset(value, 0, LTE_BUFFER_LENGTH);
    snprintf(value, LTE_BUFFER_LENGTH, "%d,%ld", _client_id, strlen(_payload));

    ret = sim_a7860c_set_mqtt_value_with_fill_header(RES_TYPE_OK, _client_id,
                                                     "CMQTTPAYLOAD", value, _payload, 2000);
    if(ret < 0){
        LOG_ERR(TAG, "Set client %d payload FAILED", _client_id);
        return ret;
    }

    if(_timeout < 60 || _timeout > 180){
        _timeout = 60;
    }
    memset(value, 0, LTE_BUFFER_LENGTH);
    snprintf(value, LTE_BUFFER_LENGTH, "%d,%d,%d", _client_id, _qos, _timeout);

    return sim_a7860c_set_mqtt_value_echo_res(_client_id, "CMQTTPUB", value, 5000);
}

static int32_t parser_mqtt_subscribed_msg(sim_a7680c_driver_impl_t* this){
    char* p_topic = NULL;
    char* p_payload = NULL;
    int mqtt_client_id = 0;
    int payload_len = 0;
    int topic_len = 0;
    int ret = 0;

    char* token = strstr(this->m_buffer, "+CMQTTRXSTART:");
    if(!token){
        LOG_ERR(TAG, "Data missing header field");
        return -1;
    }

    ret = sscanf(token, "+CMQTTRXSTART: %d,%d,%d\r\n", &mqtt_client_id, &topic_len, &payload_len);

    if(ret != 3){
        LOG_ERR(TAG, "Header missing info field");
        return -1;
    }

    LOG_INF(TAG, "Parse msg header - id: %d, topic len: %d, payload len: %d", mqtt_client_id, topic_len, payload_len);

    p_payload = strstr(this->m_buffer, "+CMQTTRXPAYLOAD:");
    p_topic = strstr(this->m_buffer, "+CMQTTRXTOPIC:");
    if(!p_topic){
        LOG_ERR(TAG, "Data missing topic field");
        return -1;
    }

    p_topic = strtok(p_topic, "\n");
    p_topic = strtok(NULL, "\n");
    p_topic = strtok(NULL, "\r");
    LOG_INF(TAG, "Parsed topic: %s", p_topic);

    if(!p_payload){
        LOG_ERR(TAG, "This msg not include payload field");
    }else{
        p_payload = strtok(p_payload, "\n");
        p_payload = strtok(NULL, "\n");
        p_payload = strtok(NULL, "\r");
        LOG_INF(TAG, "Parsed msg: %s", p_topic);
    }
    if(this->m_mqtt.m_cb.msg_coming_cb){
        this->m_mqtt.m_cb.msg_coming_cb(mqtt_client_id, p_topic, p_payload, payload_len, this->m_mqtt.m_arg);
    }
    return 1;
}

int32_t sim_a7860c_mqtt_polling(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }

    int ret = -1;

    if(modem_get_recv_byte_available(this->m_modem) <= 0){
        return 0;
    }

    memset(this->m_buffer, 0, LTE_BUFFER_LENGTH);
    ret = modem_read_until_string(this->m_modem, END_OF_LINE, this->m_buffer, LTE_BUFFER_LENGTH, 10);
    if(ret > 0){
        LOG_INF(TAG, "Load a payload %d byte", ret);
        if(strstr(this->m_buffer, "+CMQTTRXSTART:")){
            LOG_INF(TAG, "Start parse a subscribed msg");
            uint32_t time_end = get_tick_count() + TIMEOUT_POLL_MSG;
            while(time_end > get_tick_count()){
                modem_read_until_string(this->m_modem, END_OF_LINE,
                                              this->m_buffer + strlen(this->m_buffer),
                                              LTE_BUFFER_LENGTH - strlen(this->m_buffer), TIMEOUT_POLL_MSG);

                if(strstr(this->m_buffer, "+CMQTTRXEND:")){
                    LOG_INF(TAG, "Parsed SUCCEED a subscribed msg");
                    parser_mqtt_subscribed_msg(this);
                    break;
                }
            }
        }
    }
    return 0;
}

/*************************************** MQTT COMMANDS ************************************/

int32_t sim_a786c_start_http_mode(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    int ret = sim_a7860c_send_cmd(this, "HTTPINIT", 1000);
    if(ret < 0){
        return ret;
    }
    return sim_a7860c_set_value_res_ok(this, "HTTPPARA", "\"READMODE\",1", 1000);
}

int32_t sim_a786c_stop_http_mode(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    return sim_a7860c_send_cmd(this, "HTTPTERM", 1000);
}

HTTP_CODE sim_a786c_http_download_full_content(const char* _url){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!_url || !strlen(_url)){
        return -1;
    }
    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "\"URL\",\"%s\"", _url);

    int ret = sim_a7860c_set_value_res_ok(this, "HTTPPARA", value, 5000);
    if(ret < 0){
        return -1;
    }
    delay(3);
    const char* p_res = sim_a7860c_set_value_res_echo(this, "HTTPACTION", "0", 5000);
    if(!p_res){
        return -1;
    }

    int http_code = 0, content_size = 0;
    if(sscanf(p_res, "+HTTPACTION: 0,%d,%d\r\n", &http_code, &content_size) != 2){
        return -1;
    }

    LOG_INF(TAG, "Load content form %s with http code %d, content size %d", http_code, content_size);
    return http_code;
}

HTTP_CODE sim_a786c_http_load_content(uint32_t _offset, uint32_t _size, char* _buff){
    sim_a7680c_driver_impl_t* this = &g_instance;
    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "AT+HTTPREAD=%d,%d\r\n", _offset, _size);

    int ret = modem_send_cmd(this->m_modem, value, "+HTTPREAD: 0\r\n", ERROR_RES, 3000);
    if(ret < 0){
        LOG_ERR(TAG, "Load content range failed");
        return -1;
    }
    char *p_res = modem_get_buff(this->m_modem);
    p_res = strsep(&p_res, "\r\n"); // "OK"
    p_res = strsep(&p_res, "\r\n"); // an empty line
    p_res = strsep(&p_res, "\r\n"); // HTTPREAD: <size>
    p_res = strsep(&p_res, "\r\n"); // payload

    LOG_INF(TAG, "Load content range %d - %d is: %s", _offset, _offset+_size, p_res);
    if(_buff){
        memcpy(_buff, p_res, v_min_off(strlen(p_res), _size));
    }
    return strlen(p_res);
}
