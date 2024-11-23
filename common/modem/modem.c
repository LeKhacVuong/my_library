//
// Created by vuonglk on 22/11/2024.
//

#include <malloc.h>
#include <string.h>
#include "stdint.h"
#include "modem.h"
#include "sm_logger.h"
#include "elapsed_timer.h"

#define TAG "modem"

typedef struct{
    uint32_t m_buff_len;
    char* m_buff;
    modem_if_fn_t* m_fn;
    volatile uint8_t m_lock; ///TODO later
}modem_impl_t;

modem_t* modem_create(modem_if_fn_t* _fn, uint32_t _buffer_len){
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
    this->m_fn = _fn;
    this->m_lock = 0;
    return this;
}

int32_t modem_send_cmd(modem_t* _modem, const char* _cmd, const char* _res_ok, const char* _res_fail, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_fn->modem_send ){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    if(this->m_fn->modem_send(_cmd, strlen(_cmd), _timeout, this->m_fn->m_arg) < 0){
        LOG_ERR(TAG, "Send cmd FAILED");
        return -1;
    }

    if(!this->m_fn->modem_recv){
        LOG_ERR(TAG, "Cannot recv data");
        return -1;
    }

    elapsed_timer_t wait_timeout;
    elapsed_timer_resetz(&wait_timeout, _timeout);
    memset(this->m_buff, 0, this->m_buff_len);
    uint32_t cur_index = 0;
    int8_t res = -1;

    while (elapsed_timer_get_remain(&wait_timeout)){
        uint32_t len = this->m_fn->modem_recv(this->m_buff + cur_index, this->m_buff_len - cur_index, 10, this->m_fn->m_arg);
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
    if(!this || !this->m_fn->modem_recv ){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    elapsed_timer_t wait_timeout;
    elapsed_timer_resetz(&wait_timeout, _timeout);
    memset(this->m_buff, 0, this->m_buff_len);
    uint32_t cur_index = 0;

    while (elapsed_timer_get_remain(&wait_timeout)){
        uint32_t len = this->m_fn->modem_recv(this->m_buff + cur_index, 1, 1, this->m_fn->m_arg);
        if(len > 0){
            cur_index += len;
            if(this->m_buff[cur_index] == _ch){
                memcpy(_buff, this->m_buff, cur_index + 1); //End of string '\0'
                if(this->m_fn->modem_clear){
                    this->m_fn->modem_clear(this->m_fn->m_arg);
                }
                return cur_index + 1;
            }
        }
    }
    return -1;
}


int32_t modem_read_until_string(modem_t* _modem, char* _buff, const char* _ch, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_fn->modem_recv ){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    elapsed_timer_t wait_timeout;
    elapsed_timer_resetz(&wait_timeout, _timeout);
    memset(this->m_buff, 0, this->m_buff_len);
    uint32_t cur_index = 0;

    while (elapsed_timer_get_remain(&wait_timeout)){
        uint32_t len = this->m_fn->modem_recv(this->m_buff + cur_index, 1, 1, this->m_fn->m_arg);
        if(len > 0){
            cur_index += len;
            if(this->m_buff[cur_index] == _ch){
                memcpy(_buff, this->m_buff, cur_index + 1); //End of string '\0'
                if(this->m_fn->modem_clear){
                    this->m_fn->modem_clear(this->m_fn->m_arg);
                }
                return cur_index + 1;
            }
        }
    }
    return -1;
}

const char* modem_get_buff(modem_t* _modem){
    modem_impl_t* this = _modem;
    if(!this){
        return NULL;
    }
    return this->m_buff;
}

int32_t modem_send_raw_data(modem_t* _modem, const char* _buff, uint32_t _len, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_fn->modem_send ){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    return this->m_fn->modem_send(_buff, _len, _timeout, this->m_fn->m_arg);
}

int32_t modem_recv_raw_data(modem_t* _modem, char* _buff, uint32_t _len, uint32_t _timeout){
    modem_impl_t* this = _modem;
    if(!this || !this->m_fn->modem_recv ){
        LOG_ERR(TAG, "Invalid argument");
        return -1;
    }

    return this->m_fn->modem_recv(_buff, _len, _timeout, this->m_fn->m_arg);
}

int32_t modem_reset_data(modem_t* _modem){
    modem_impl_t* this = _modem;
    if(!this){
        return -1;
    }
    if(this->m_fn->modem_clear){
        this->m_fn->modem_clear(this->m_fn->m_arg);
    }
    return 0;
}

int32_t modem_restart_module(modem_t* _modem, uint32_t _duration){
    modem_impl_t* this = _modem;
    if(!this || this->m_fn->modem_reset){
        return -1;
    }
    return 0;
}
