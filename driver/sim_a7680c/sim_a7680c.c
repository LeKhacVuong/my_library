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

    ret = modem_send_cmd(this->m_modem, buff, SUCCESS_RES, FAIL_RES, 500);
    if(ret < 0){
        LOG_ERR(TAG, "Set sim send CMD \"%s\" FAILED",_cmd);
        return -1;
    }

    LOG_INF(TAG, "Set sim send CMD \"%s\" SUCCEED",_cmd);
    return ret;
}

static int32_t sim_a7860c_set_value(sim_a7680c_driver_impl_t* this, const char* _key, DATA_TYPE _type, const char* _value, uint32_t _timeout){
    int32_t ret = -1;

    char buff[LTE_BUFFER_LENGTH] = {0,};

    if(_type == DATA_STRING){
        snprintf(buff, LTE_BUFFER_LENGTH, "AT+%s=\"%s\"\r\n", _key, _value);
    }else if(_type == DATA_RAW){
        snprintf(buff, LTE_BUFFER_LENGTH, "AT+%s=%s\r\n", _key, _value);
    }

    ret = modem_send_cmd(this->m_modem, buff, SUCCESS_RES, FAIL_RES, 500);
    if(ret < 0){
        LOG_ERR(TAG, "Set sim key \"%s\" FAILED",_key);
        return -1;
    }

    LOG_INF(TAG, "Set sim key \"%s\" to \"%s\" SUCCEED",_key, _value);
    return ret;
}


static int32_t sim_a7860c_get_value(sim_a7680c_driver_impl_t* this, const char* _key, bool _qst_mrk, char* _buff, uint32_t _max_len, uint32_t _timeout){
    int32_t ret = -1;

    char buff[LTE_BUFFER_LENGTH] = {0,};

    snprintf(buff, LTE_BUFFER_LENGTH, "AT+%s%s\r\n", _key, _qst_mrk?"?":"");

    ret = modem_send_cmd(this->m_modem, buff, SUCCESS_RES, FAIL_RES, 500);
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

int32_t sim_a7680c_init_driver(v_serial_t* _serial){
    sim_a7680c_driver_impl_t* this = &g_instance;

    this->m_modem = modem_create_default(LTE_BUFFER_LENGTH, _serial);
    if(!this->m_modem){
        return -1;
    }
    return 0;
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

int32_t sim_a7860c_start_mqtt_mode(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }
    return sim_a7860c_send_cmd(this, "AT+CMQTTSTART", 1000);
}

int32_t sim_a7860c_stop_mqtt_mode(){
    sim_a7680c_driver_impl_t* this = &g_instance;
    if(!this->m_modem){
        return -1;
    }
    return sim_a7860c_send_cmd(this, "AT+CMQTTSTOP", 1000);
}





/*************************************** MQTT COMMANDS ************************************/

