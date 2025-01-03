//
// Created by vuonglk on 29/03/2023.
//

#ifndef SELEX_BOOTLOADER_SM_DESKTOP_CANOPEN_IF_H
#define SELEX_BOOTLOADER_SM_DESKTOP_CANOPEN_IF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void (*sm_canopen_if_receive_callback_fn)(const uint32_t, uint8_t*);

typedef struct sm_canopen_if sm_canopen_if_t;

sm_canopen_if_t* sm_dk_canopen_if_create_default(sm_canopen_if_receive_callback_fn _fn);

int32_t sm_dk_canopen_if_send(sm_canopen_if_t* _self, uint32_t _cob_id, const uint8_t* _data, int32_t _len);
int32_t sm_dk_canopen_if_process(sm_canopen_if_t* _self);

#ifdef __cplusplus
};
#endif

#endif //SELEX_BOOTLOADER_SM_DESKTOP_CANOPEN_IF_H
