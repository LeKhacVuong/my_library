//
// Created by lekhacvuong on 12/1/24.
//

#include <malloc.h>
#include <string.h>
#include "hc_05_base.h"
#include "v_modem.h"
#include "sm_logger.h"
#include "v_math.h"

#define TAG "hc_05_base"

#define HC_05_BUFF_LEN 512

typedef struct {
    hc_05_proc_t m_proc;

    v_modem_t * m_modem;
    hc_05_ctrl_enable_pin m_ctrl_en_pin;
    void* m_arg;
}hc_05_impl_t;

#define OK_RES     "OK\r\n"
#define ERROR_RES        "ERROR:\r\n"

#define FAIL_RES_FORM   "ERROR:(%d)\r\n"

static int32_t hc_05_start_config(hc_05_impl_t* this){
    if(this->m_ctrl_en_pin){
        return this->m_ctrl_en_pin(1, this->m_arg);
    }
    return -1;
}

static int32_t hc_05_stop_config(hc_05_impl_t* this){
    if(this->m_ctrl_en_pin){
        return this->m_ctrl_en_pin(0, this->m_arg);
    }
    return -1;
}

static int32_t hc_05_set_value(hc_05_impl_t* this, const char* _key, const char* _value){
    int32_t ret = -1;
    hc_05_start_config(this);

    char buff[HC_05_BUFF_LEN] = {0,};

    if(_value){
        snprintf(buff, HC_05_BUFF_LEN, "AT+%s=\"%s\"\r\n", _key, _value);
    }else{
        snprintf(buff, HC_05_BUFF_LEN, "AT+%s\r\n", _key);
    }
    ret = modem_send_cmd(this->m_modem, buff, OK_RES, ERROR_RES, 500);
    if(ret < 0){
        int ret_code = -1;
        sscanf(modem_get_buff(this->m_modem), FAIL_RES_FORM, &ret_code);
        LOG_ERR(TAG, "Set name ERROR, ret %d", ret_code);
        goto EXIT;
    }

    LOG_INF(TAG, "Set HC 05 key \"%s\" to \"%s\" SUCCEED",_key, _value?_value:"_");

    EXIT:
    hc_05_stop_config(this);
    return ret;
}


static int32_t hc_05_get_value(hc_05_impl_t* this, const char* _key, char* _buff, uint32_t _max_len){
    int32_t ret = -1;
    hc_05_start_config(this);

    char buff[HC_05_BUFF_LEN] = {0,};
    snprintf(buff, HC_05_BUFF_LEN, "AT+%s?\r\n", _key);
    ret = modem_send_cmd(this->m_modem, buff, OK_RES, ERROR_RES, 500);
    if(ret < 0){
        int ret_code = -1;
        sscanf(modem_get_buff(this->m_modem), FAIL_RES_FORM, &ret_code);
        LOG_ERR(TAG, "Get \"%s\" ERROR, ret %d", _key, ret_code);
        goto EXIT;
    }

    char value[HC_05_BUFF_LEN] = {0,};
    ret = sscanf(modem_get_buff(this->m_modem), "+NAME:%s\r\n", value);
    memcpy(_buff, value, v_min_off(_max_len, strlen(value));
    LOG_INF(TAG, "HC 05 key \"%s\" is: %s", _key, value);

    EXIT:
    hc_05_stop_config(this);
    return ret;
}



static int32_t hc_05_set_name(hc_05_proc_t* _this, const char* _data){
    hc_05_impl_t* this = _this;
    if(!this || !this->m_modem){
        return -1;
    }

    return hc_05_set_value(this, "NAME", _data);
}

static int32_t hc_05_get_name(hc_05_proc_t* _this, char* _buff, uint32_t _max_len){
    hc_05_impl_t* this = _this;
    if(!this || !this->m_modem){
        return -1;
    }

    return hc_05_get_value(this, "NAME", _buff, _max_len);
}

static int32_t hc_05_set_pass(hc_05_proc_t* _this, const char* _data){
    hc_05_impl_t* this = _this;
    if(!this || !this->m_modem){
        return -1;
    }

    return hc_05_set_value(this, "PSWD", _data);;
}

static int32_t hc_05_get_pass(hc_05_proc_t* _this, char* _buff, uint32_t _max_len){
    hc_05_impl_t* this = _this;
    if(!this || !this->m_modem){
        return -1;
    }

    return hc_05_get_value(this, "PSWD", _buff, _max_len);
}

static int32_t hc_05_set_addr(hc_05_proc_t* _this, const char* _data){
    hc_05_impl_t* this = _this;
    if(!this || !this->m_modem){
        return -1;
    }
    int32_t ret = -1;

    return ret;
}

static int32_t hc_05_get_addr(hc_05_proc_t* _this, char* _buff, uint32_t _len){
    hc_05_impl_t* this = _this;
    if(!this || !this->m_modem){
        return -1;
    }
    int32_t ret = -1;

    return ret;
}

static int32_t hc_05_set_role(hc_05_proc_t* _this, uint8_t _role){
    hc_05_impl_t* this = _this;
    if(!this || !this->m_modem){
        return -1;
    }
    int32_t ret = -1;

    return ret;
}

static int32_t hc_05_get_role(hc_05_proc_t* _this){
    hc_05_impl_t* this = _this;
    if(!this || !this->m_modem){
        return -1;
    }
    int32_t ret = -1;

    return ret;
}

static int32_t hc_05_set_baud(hc_05_proc_t* _this, uint32_t _baud){
    hc_05_impl_t* this = _this;
    if(!this || !this->m_modem){
        return -1;
    }
    int32_t ret = -1;

    return ret;
}

static int32_t hc_05_get_baud(hc_05_proc_t* _this){
    hc_05_impl_t* this = _this;
    if(!this || !this->m_modem){
        return -1;
    }
    int32_t ret = -1;

    return ret;
}

static int32_t hc_05_get_version(hc_05_proc_t* _this, char* _buff, uint32_t _len){
    hc_05_impl_t* this = (hc_05_impl_t *) _this;
    if(!this || !this->m_modem){
        return -1;
    }
    int32_t ret = -1;

    return ret;
}

static hc_05_proc_t g_proc = {
        .get_name = hc_05_get_name,
        .set_name = hc_05_set_name,
        .get_role = hc_05_get_role,
        .set_role = hc_05_set_role,
        .get_addr  = hc_05_get_addr,
        .set_addr = hc_05_set_addr,
        .get_pass = hc_05_get_pass,
        .set_pass = hc_05_set_pass,

};


hc_05_proc_t* hc_05_create(v_serial_t* _serial, hc_05_ctrl_enable_pin _fn, void* _arg){
    hc_05_impl_t* this = malloc(sizeof(hc_05_impl_t));
    if(!this){
        return NULL;
    }

    this->m_modem = modem_create_default(512, _serial);
    if(!this->m_modem){
        LOG_ERR(TAG, "Create modem FAILED");
        free(this);
        return NULL;
    }
    this->m_ctrl_en_pin = _fn;
    this->m_arg = _arg;
    this->m_proc = g_proc;
    return &this->m_proc;
}



