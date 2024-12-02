//
// Created by lekhacvuong on 11/30/24.
//

#include <string.h>
#include <stdbool.h>
#include "sim_a7680c.h"
#include "v_modem.h"
#include "sm_logger.h"


#include "v_math.h"

#define TAG "sim_a7680c"

#define LTE_BUFFER_LENGTH 512

#define SUCCESS_RES     "OK\r\n"
#define FAIL_RES        "ERROR"

#define END_OF_LINE     "\r\n"

typedef struct{
    v_modem_t* m_modem;
    char m_buffer[LTE_BUFFER_LENGTH];
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

    ret = modem_send_cmd(this->m_modem, buff, SUCCESS_RES, FAIL_RES, _timeout);
    if(ret < 0){
        LOG_ERR(TAG, "Set sim send CMD \"%s\" FAILED",_cmd);
        return -1;
    }

    LOG_INF(TAG, "Set sim send CMD \"%s\" SUCCEED",_cmd);
    return ret;
}

static int32_t sim_a7860c_set_value(sim_a7680c_driver_impl_t* this,
                                    const char* _key,
                                    DATA_TYPE _type,
                                    const char* _value,
                                    uint32_t _timeout){
    int32_t ret = -1;

    char buff[LTE_BUFFER_LENGTH] = {0,};

    if(_type == DATA_STRING){
        snprintf(buff, LTE_BUFFER_LENGTH, "AT+%s=\"%s\"\r\n", _key, _value);
    }else if(_type == DATA_RAW){
        snprintf(buff, LTE_BUFFER_LENGTH, "AT+%s=%s\r\n", _key, _value);
    }

    ret = modem_send_cmd(this->m_modem, buff, SUCCESS_RES, FAIL_RES, _timeout);
    if(ret < 0){
        LOG_ERR(TAG, "Set sim key \"%s\" FAILED",_key);
        return -1;
    }

    if(_type == DATA_STRING){
        LOG_INF(TAG, "Set sim key \"%s\" to \"%s\" SUCCEED",_key, _value);
    }else if(_type == DATA_RAW){
        LOG_INF(TAG, "Set sim key \"%s\" to %s SUCCEED",_key, _value);
    }

    return ret;
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

    ret = modem_send_cmd(this->m_modem, buff, SUCCESS_RES, FAIL_RES, _timeout);
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
        return sim_a7860c_set_value(this, "CIPRXGET", DATA_RAW, "1", 1000);
    }else{
        return sim_a7860c_set_value(this, "CIPRXGET", DATA_RAW, "0", 1000);
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

    return sim_a7860c_set_value(this, "CIPOPEN", DATA_RAW, value, 1000);
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
    return sim_a7860c_set_value(this, "CIPCLOSE", DATA_RAW, value, 1000);
}

/*************************************** MQTT COMMANDS ************************************/

static int32_t sim_a7860c_set_mqtt_value(sim_a7680c_driver_impl_t* this,
                                         MQTT_CLIENT_ID _client_id,
                                         const char* _key,
                                         const char* _value,
                                         uint32_t _timeout){
    int32_t ret = -1;
    char buff[LTE_BUFFER_LENGTH] = {0,};

    snprintf(buff, LTE_BUFFER_LENGTH, "AT+%s=%s\r\n", _key, _value);
    ret = modem_send_cmd(this->m_modem, buff, SUCCESS_RES, FAIL_RES, _timeout);
    if(ret < 0){
        LOG_ERR(TAG, "Set mqtt key \"%s\" FAILED by not OK",_key);
        return -1;
    }

    int id = 0, ret_code = 0;

    char* p_data = strrchr(modem_get_buff(this->m_modem), '+');  // Find last '+'
    char res_format[LTE_BUFFER_LENGTH] = {0,};
    snprintf(res_format, LTE_BUFFER_LENGTH, "+%s: %s,%s\r\n", _key, "%d", "%d");
    ret = sscanf(p_data, res_format, &id, &ret_code);
    if(ret == 2 && id == _client_id){
        if(ret_code == 0){
            LOG_INF(TAG, "Set mqtt key \"%s\" to: %s SUCCEED", _key, _value);
            return 0;
        }else{
            LOG_INF(TAG, "Set mqtt key \"%s\" to: %s OK, but sub err is %d", _key, _value, ret_code);
            return -1;
        }
    }
    return -1;
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

int32_t sim_a7860c_init_mqtt_client(MQTT_CLIENT_ID _client_id, const char* _client_name, MQTT_SERVER_TYPE _server_type){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER || _server_type >= MQTT_SERVER_TYPE_NUMBER){
        return -1;
    }

    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "%d,\"%s\",%d", _client_id, _client_name, _server_type);

    return sim_a7860c_set_value(this, "CMQTTACCQ", DATA_RAW, value, 1000);
}

int32_t sim_a7860c_set_will_msg(MQTT_CLIENT_ID _client_id, const char* _topic, const char* _msg, uint8_t _qos){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER || _qos > 2){
        return -1;
    }

    int ret = -1;

    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "AT+CMQTTWILLTOPIC=%d,%ld\r\n", _client_id, strlen(_topic));

    ret = modem_send_cmd(this->m_modem, value, ">\r\n", FAIL_RES, 500);
    if(ret < 0){
        LOG_ERR(TAG, "Set will topic header FAILED");
        return -1;
    }
    memset(value, 0, LTE_BUFFER_LENGTH);
    snprintf(value, LTE_BUFFER_LENGTH, "%s\r\n", _topic);
    ret = modem_send_cmd(this->m_modem, value, SUCCESS_RES, FAIL_RES, 500);
    if(ret < 0){
        LOG_ERR(TAG, "Set will topic data FAILED");
        return -1;
    }

    LOG_INF(TAG, "Set will topic: \"%s\" SUCCEED", _topic);

    memset(value, 0, LTE_BUFFER_LENGTH);
    snprintf(value, LTE_BUFFER_LENGTH, "AT+CMQTTWILLMSG=%d,%ld,%d\r\n", _client_id, strlen(_msg), _qos);
    ret = modem_send_cmd(this->m_modem, value, ">\r\n", FAIL_RES, 500);
    if(ret < 0){
        LOG_ERR(TAG, "Set will msg header FAILED");
        return -1;
    }
    memset(value, 0, LTE_BUFFER_LENGTH);
    snprintf(value, LTE_BUFFER_LENGTH, "%s\r\n", _msg);
    ret = modem_send_cmd(this->m_modem, value, SUCCESS_RES, FAIL_RES, 500);
    if(ret < 0){
        LOG_ERR(TAG, "Set will msg data FAILED");
        return -1;
    }

    LOG_INF(TAG, "Set will msg: \"%s\" SUCCEED", _msg);
    return 0;
}

int32_t sim_a7860c_connect(MQTT_CLIENT_ID _client_id,
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

    return sim_a7860c_set_mqtt_value(this, _client_id, "CMQTTCONNECT", value, 5000);
}

int32_t sim_a7860c_check_connection(MQTT_CLIENT_ID _client_id){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER){
        return -1;
    }

    return 0;
}


int32_t sim_a7860c_disconnect(MQTT_CLIENT_ID _client_id, uint32_t _timeout){
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
    return sim_a7860c_set_mqtt_value(this, _client_id, "CMQTTDISC", value, 5000);
}


int32_t sim_a7860c_subscribe(MQTT_CLIENT_ID _client_id, const char* _topic, uint8_t _qos){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem || _client_id >= MQTT_CLIENT_ID_NUMBER || _qos > 2){
        return -1;
    }

    int ret = -1;

    char value[LTE_BUFFER_LENGTH] = {0,};
    snprintf(value, LTE_BUFFER_LENGTH, "AT+CMQTTSUB=%d,%ld,%d\r\n", _client_id, strlen(_topic), _qos);

    ret = modem_send_cmd(this->m_modem, value, ">\r\n", FAIL_RES, 500);
    if(ret < 0){
        LOG_ERR(TAG, "Set sub topic header FAILED");
        return -1;
    }
    memset(value, 0, LTE_BUFFER_LENGTH);
    snprintf(value, LTE_BUFFER_LENGTH, "%s\r\n", _topic);
    ret = modem_send_cmd(this->m_modem, value, SUCCESS_RES, FAIL_RES, 500);
    if(ret < 0){
        LOG_ERR(TAG, "Set sub topic data FAILED");
        return -1;
    }

    LOG_INF(TAG, "Set sub topic: \"%s\" SUCCEED", _topic);

    memset(value, 0, LTE_BUFFER_LENGTH);
    snprintf(value, LTE_BUFFER_LENGTH, "%d,%ld,%d\r\n", _client_id, strlen(_topic), _qos);
    return sim_a7860c_set_mqtt_value(this, _client_id, "CMQTTSUB", value, 5000);
}


