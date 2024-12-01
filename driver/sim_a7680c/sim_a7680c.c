//
// Created by lekhacvuong on 11/30/24.
//

#include "sim_a7680c.h"
#include "v_modem.h"
#include "sm_logger.h"

#define LTE_BUFFER_LENGTH 512

#define TAG "sim_a7680c"

#define SUCCESS_RET     "OK\r\n"
#define FAIL_RED        "ERROR\r\n"

typedef struct{
    v_modem_t* m_modem;
    char m_buffer[LTE_BUFFER_LENGTH];
}sim_a7680c_driver_impl_t;

static sim_a7680c_driver_impl_t g_instance;

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
        LOG_ERR(TAG, "Modem Invalid");
        return -1;
    }

    return modem_send_cmd(this->m_modem, ":", "", FAIL_RED, 1000);
}

int32_t sim_a7680c_process(){

    return 0;
}