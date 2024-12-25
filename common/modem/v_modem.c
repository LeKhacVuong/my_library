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
    char m_buff[2048];
    volatile uint8_t m_lock; ///TODO later
}modem_impl_t;

static modem_impl_t g_modem;

typedef enum{
    DATA_RAW,
    DATA_INT,
    DATA_STRING
}DATA_TYPE;

v_modem_t* modem_create_default(uint32_t _buffer_len, v_serial_t* _serial){
    if(!_serial){
        return NULL;
    }
    g_modem.m_buff_len = 1024;
    g_modem.m_lock = 0;
    g_modem.m_serial = _serial;
    g_modem.m_serial->flush(g_modem.m_serial, SERIAL_TX_RX);
    return &g_modem;
}

v_modem_t* modem_create(uint32_t _buffer_len, v_serial_t* _serial){
    if(!_serial){
        return NULL;
    }

    modem_impl_t* this = malloc(sizeof(modem_impl_t));
    if(!this){
        LOG_ERR(TAG, "Over heap for modem instance!!");
        return NULL;
    }
    this->m_buff_len = 1024;
    this->m_lock = 0;
    this->m_serial = _serial;
    this->m_serial->flush(this->m_serial, SERIAL_TX_RX);
    return this;
}

int32_t modem_send_cmd(v_modem_t* _modem, const char* _cmd, const char* _res_ok, const char* _res_fail, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial ){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }
    v_serial_t* serial = this->m_serial;
    serial->flush(serial, SERIAL_TX_RX);

    if(serial->send(serial, _cmd, strlen(_cmd)) < 0){
        LOG_ERR(TAG, "Send cmd FAILED");
        return -1;
    }
//    LOG_INF(TAG, "Send modem cmd %s", _cmd);
    elapsed_timer_t wait_timeout;
    elapsed_timer_resetz(&wait_timeout, _timeout);
    memset(this->m_buff, 0, this->m_buff_len);
    int32_t cur_len = 0;
    int8_t res = -1;

    while (elapsed_timer_get_remain(&wait_timeout)){
        int32_t len = serial->read_blocking(serial, this->m_buff + cur_len, this->m_buff_len - cur_len, 5);
        if(len > 0){
            cur_len += len;
            if(cur_len >= this->m_buff_len){
                return -1;
            }
            if(this->m_buff[cur_len-1] == '\n'){
                if(strstr(this->m_buff, _res_ok) != NULL){
//                LOG_INF(TAG, "Cmd %s ret SUCCEED: >>|| %s ||<<", _cmd, this->m_buff);
                    return 0;
                }
                if(strstr(this->m_buff, _res_fail) != NULL) {
                    LOG_WRN(TAG, "Cmd %s ret FAILED: >>|| %s ||<<", _cmd, this->m_buff);
                    return -2;
                }
            }
        }
    }
    LOG_ERR(TAG, "Cmd %s ret TIMEOUT", _cmd);
    return res;
}

int32_t modem_read_until_char(v_modem_t* _modem, char _ch, char* _buff, uint32_t _max_len, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    v_serial_t* serial = this->m_serial;

    elapsed_timer_t wait_timeout;
    elapsed_timer_resetz(&wait_timeout, _timeout);
    memset(this->m_buff, 0, this->m_buff_len);
    int32_t cur_len = 0;

    while (elapsed_timer_get_remain(&wait_timeout)){
        int32_t len = serial->read_blocking(serial, this->m_buff + cur_len, 1, 0);
        if(len > 0){
            cur_len += 1;
            if(cur_len >= this->m_buff_len){
                return -1;
            }
            if(this->m_buff[cur_len-1] == _ch){
                uint32_t true_len = v_min_off(cur_len + 1, _max_len);
                memcpy(_buff, this->m_buff, true_len); //End of string '\0'
                return true_len;
            }
        }
    }
    return -1;
}


int32_t modem_read_until_string(v_modem_t* _modem, const char* _str, char* _buff, uint32_t _max_len, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    v_serial_t* serial = this->m_serial;

    elapsed_timer_t wait_timeout;
    elapsed_timer_resetz(&wait_timeout, _timeout);
    memset(this->m_buff, 0, this->m_buff_len);
    int32_t cur_len = 0;

    while (elapsed_timer_get_remain(&wait_timeout)){
        int32_t len = serial->read(serial, this->m_buff + cur_len, 1);
        if(len > 0){
            cur_len += len;
            if(cur_len >= this->m_buff_len){
                return -1;
            }
            if(strstr(this->m_buff, _str)){
                uint32_t true_len = v_min_off(cur_len + 1, _max_len);
                memcpy(_buff, this->m_buff, true_len); //End of string '\0'
                return true_len;
            }
        }
    }
    return -1;
}

char* modem_polling_data_stringz(v_modem_t* _modem, const char* _start, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial ){
        LOG_ERR(TAG, "Invalid argument");
        return NULL;
    }

    v_serial_t* serial = this->m_serial;

    elapsed_timer_t wait_timeout;
    elapsed_timer_resetz(&wait_timeout, _timeout);
    memset(this->m_buff, 0, this->m_buff_len);
    int32_t cur_len = 0;

    while (elapsed_timer_get_remain(&wait_timeout)){
        int32_t len = serial->read_blocking(serial, this->m_buff + cur_len, this->m_buff_len - cur_len, 10);
        if(len > 0){
            cur_len += len;
            const char* data = strstr(this->m_buff, _start);
            if(data != NULL){
                LOG_INF(TAG, "Polling data %s ret SUCCEED, data: %s", _start, data);
                return data;
            }
        }
    }
    return NULL;
}

char* modem_get_buff(v_modem_t* _modem){
    modem_impl_t* this = _modem;
    if(!this){
        return NULL;
    }
    return this->m_buff;
}

int32_t modem_send_raw_data(v_modem_t* _modem, const char* _buff, uint32_t _len, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    return this->m_serial->send(this->m_serial, _buff, _len);
}

int32_t modem_recv_raw_data(v_modem_t* _modem, char* _buff, uint32_t _len, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    return this->m_serial->read_blocking(this->m_serial, _buff, _len, _timeout);
}

uint32_t modem_get_recv_byte_available(v_modem_t* _modem){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    return this->m_serial->bytes_available(this->m_serial);
}

int32_t modem_reset_data(v_modem_t* _modem){
    modem_impl_t* this = _modem;
    if(!this || !this->m_serial){
        return -1;
    }
    this->m_serial->flush(this->m_serial, SERIAL_TX);
    return 0;
}

int32_t modem_restart_module(v_modem_t* _modem, uint32_t _duration){
    modem_impl_t* this = _modem;

    return 0;
}
