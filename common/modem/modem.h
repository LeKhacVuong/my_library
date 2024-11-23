//
// Created by vuonglk on 22/11/2024.
//

#ifndef LIBS_STORAGE_MODEM_H
#define LIBS_STORAGE_MODEM_H

typedef void modem_t;

typedef struct{
    int(*modem_recv)(char* buff, uint32_t max_len, uint32_t timeout, void* arg);        // recv bytes
    int(*modem_clear)(void* arg);                                                       // clear recv buff
    int(*modem_send)(const char* buff, uint32_t len, uint32_t timeout, void* arg);      // send bytes
    int(*modem_reset)(uint8_t state, void* arg);                                        // hardware reset
    void* m_arg;
}modem_if_fn_t;

modem_t* modem_create(modem_if_fn_t* _fn, uint32_t _buffer_len);

int32_t modem_send_cmd(modem_t* _modem, const char* _cmd, const char* _res_ok, const char* _res_fail, uint32_t _timeout);

int32_t modem_read_until_char(modem_t* _modem, char* _buff, char _ch, uint32_t _timeout);

int32_t modem_read_until_string(modem_t* _modem, char* _buff, const char* _ch, uint32_t _timeout);

const char* modem_get_buff(modem_t* _modem);

int32_t modem_send_raw_data(modem_t* _modem, const char* _buff, uint32_t _len, uint32_t _timeout);

int32_t modem_recv_raw_data(modem_t* _modem, char* _buff, uint32_t _len, uint32_t _timeout);

int32_t modem_reset_data(modem_t* _modem);

int32_t modem_restart_module(modem_t* _modem, uint32_t _duration);

#endif //LIBS_STORAGE_MODEM_H
