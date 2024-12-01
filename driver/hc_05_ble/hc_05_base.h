//
// Created by lekhacvuong on 12/1/24.
//

#ifndef LIBS_STORAGE_HC_05_BASE_H
#define LIBS_STORAGE_HC_05_BASE_H
#include "stdint.h"
#include "v_serial.h"

#ifdef __cplusplus
extern "C" {

#endif

#define HC_05_ROLE_SLAVE    0
#define HC_05_ROLE_MASTER   1

typedef int32_t(*hc_05_ctrl_enable_pin)(uint8_t state, void* arg);

typedef struct hc_05_base_proc hc_05_proc_t;

struct hc_05_base_proc{
    // General interface
    int32_t (*set_name) (hc_05_proc_t* this, const char* data);
    int32_t (*get_name) (hc_05_proc_t* this, char* buff, uint32_t len);
    int32_t (*set_pass) (hc_05_proc_t* this, const char* data);
    int32_t (*get_pass) (hc_05_proc_t* this, char* buff, uint32_t len);
    int32_t (*set_addr) (hc_05_proc_t* this, const char* data);
    int32_t (*get_addr)  (hc_05_proc_t* this, char* buff, uint32_t len);
    int32_t (*set_role) (hc_05_proc_t* this, uint8_t role);
    int32_t (*get_role) (hc_05_proc_t* this);
    int32_t (*set_baud) (hc_05_proc_t* this, int32_t baud);
    int32_t (*get_baud) (hc_05_proc_t* this);
    int32_t (*get_ver)  (hc_05_proc_t* this, const char* data, uint32_t len);

    // Master only interface

};

hc_05_proc_t* hc_05_create(v_serial_t* _serial, hc_05_ctrl_enable_pin _fn, void* _arg) __wur;


#ifdef __cplusplus
}
#endif
#endif //LIBS_STORAGE_HC_05_BASE_H
