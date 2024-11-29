//
// Created by vuonglk on 22/11/2024.
//

#include <malloc.h>
#include <string.h>
#include "stdint.h"
#include "v_modem.h"
#include "sm_logger.h"
#include "elapsed_timer.h"
#include "v_math.h"
#include <stddef.h>

#define TAG "modem"

typedef struct{
    uint32_t m_buff_len;
    v_serial_t* m_serial;
    char* m_buff;
    volatile uint8_t m_lock; ///TODO later
}modem_impl_t;

static modem_impl_t g_modem;

modem_t* modem_get_default(uint32_t _buffer_len, v_serial_t* _serial){
    g_modem.m_buff = malloc(_buffer_len);
    if(!g_modem.m_buff){
        LOG_ERR(TAG, "Over heap for modem buffer!!");
        return NULL;
    }
    g_modem.m_buff_len = _buffer_len;
    g_modem.m_lock = 0;
    g_modem.m_serial = _serial;
    return &g_modem;
}

modem_t* modem_create(uint32_t _buffer_len, v_serial_t* _serial){
    modem_impl_t* this = malloc(sizeof(modem_impl_t));
    if(!this){
        LOG_ERR(TAG, "Over heap for modem instance!!");
        return NULL;
    }

    this->m_buff = malloc(_buffer_len);
    if(!this->m_buff){
        free(this);
        LOG_ERR(TAG, "Over heap for modem buffer!!");
        return NULL;
    }
    this->m_buff_len = _buffer_len;
    this->m_lock = 0;
    this->m_serial = _serial;
    return this;
}

int32_t modem_send_cmd(modem_t* _modem, const char* _cmd, const char* _res_ok, const char* _res_fail, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial ){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }
    v_serial_t* serial = this->m_serial;

    if(serial->send(serial, _cmd, strlen(_cmd)) < 0){
        LOG_ERR(TAG, "Send cmd FAILED");
        return -1;
    }

    elapsed_timer_t wait_timeout;
    elapsed_timer_resetz(&wait_timeout, _timeout);
    memset(this->m_buff, 0, this->m_buff_len);
    uint32_t cur_index = 0;
    int8_t res = -1;

    while (elapsed_timer_get_remain(&wait_timeout)){
        uint32_t len = serial->read_blocking(serial, this->m_buff + cur_index, this->m_buff_len - cur_index, 10);
        if(len > 0){
            cur_index += len;
            if(strstr(this->m_buff, _res_ok) != NULL){
                LOG_INF(TAG, "Cmd %s ret SUCCEED", _cmd);
                res = 0;
                break;
            }
            if(strstr(this->m_buff, _res_fail) != NULL) {
                LOG_INF(TAG, "Cmd %s ret FAILED", _cmd);
                res = -2;
                break;
            }
        }
    }

    return res;
}

int32_t modem_read_until_char(modem_t* _modem, char* _buff, char _ch, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    v_serial_t* serial = this->m_serial;

    elapsed_timer_t wait_timeout;
    elapsed_timer_resetz(&wait_timeout, _timeout);
    memset(this->m_buff, 0, this->m_buff_len);
    uint32_t cur_index = 0;

    while (elapsed_timer_get_remain(&wait_timeout)){
        uint32_t len = serial->read_blocking(serial, this->m_buff + cur_index, 1, 1);
        if(len > 0){
            cur_index += len;
            if(this->m_buff[cur_index] == _ch){
                memcpy(_buff, this->m_buff, cur_index + 1); //End of string '\0'
                return cur_index + 1;
            }
        }
    }
    return -1;
}


int32_t modem_read_until_string(modem_t* _modem, const char* _str, char* _buff, uint32_t _max_len, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    v_serial_t* serial = this->m_serial;

    elapsed_timer_t wait_timeout;
    elapsed_timer_resetz(&wait_timeout, _timeout);
    memset(this->m_buff, 0, this->m_buff_len);
    uint32_t cur_index = 0;

    while (elapsed_timer_get_remain(&wait_timeout)){
        uint32_t len = serial->read(serial, this->m_buff + cur_index, 1);
        if(len > 0){
            cur_index += len;
            if(this->m_buff[cur_index] == *_str){
                memcpy(_buff, this->m_buff, v_min_off(cur_index + 1, _max_len)); //End of string '\0'
                return cur_index + 1;
            }
        }
    }
    return -1;
}

char* modem_polling_data_stringz(modem_t* _modem, const char* _start, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial ){
        LOG_ERR(TAG, "Invalid argument");
        return NULL;
    }

    v_serial_t* serial = this->m_serial;

    elapsed_timer_t wait_timeout;
    elapsed_timer_resetz(&wait_timeout, _timeout);
    memset(this->m_buff, 0, this->m_buff_len);
    uint32_t cur_index = 0;

    while (elapsed_timer_get_remain(&wait_timeout)){
        uint32_t len = serial->read_blocking(serial, this->m_buff + cur_index, this->m_buff_len - cur_index, 10);
        if(len > 0){
            cur_index += len;
            const char* data = strstr(this->m_buff, _start);
            if(data != NULL){
                LOG_INF(TAG, "Polling data %s ret SUCCEED, data: %s", _start, data);
                return data;
            }
        }
    }
    return NULL;
}

char* modem_get_buff(modem_t* _modem){
    modem_impl_t* this = _modem;
    if(!this){
        return NULL;
    }
    return this->m_buff;
}

int32_t modem_send_raw_data(modem_t* _modem, const char* _buff, uint32_t _len, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    return this->m_serial->send(this->m_serial, _buff, _len);
}

int32_t modem_recv_raw_data(modem_t* _modem, char* _buff, uint32_t _len, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    return this->m_serial->read_blocking(this->m_serial, _buff, _len, _timeout);
}

int32_t modem_reset_data(modem_t* _modem){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial){
        return -1;
    }
    this->m_serial->flush(this->m_serial, SERIAL_TX);
    return 0;
}

int32_t modem_restart_module(modem_t* _modem, uint32_t _duration){
    modem_impl_t* this = _modem;

    return 0;
}
