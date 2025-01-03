//
// Created by vuonglk on 22/03/2023.
//

#ifndef SELEX_HOST_SM_BOOT_IF_H
#define SELEX_HOST_SM_BOOT_IF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SM_BOOT_IF_ENABLE       1
#define SM_BOOT_IF_DISABLE      0
#define SM_BOOT_IF_READY        SM_BOOT_IF_ENABLE
#define SM_BOOT_IF_BUSY         SM_BOOT_IF_DISABLE
#define SM_BOOT_IF_FW_SUCCESS   SM_BOOT_IF_ENABLE
#define SM_BOOT_IF_FW_FAILURE   SM_BOOT_IF_BUSY

typedef struct sm_boot_if sm_boot_if_t;

enum {
    SM_BOOT_EVENT_REQUEST_UPGRADE = 0,
    SM_BOOT_EVENT_NEW_FIRMWARE,
    SM_BOOT_EVENT_NEW_SEGMENT,
    SM_BOOT_EVENT_FINISH_DOWNLOAD,
    SM_BOOT_EVENT_FINISH_UPGRADE,
    SM_BOOT_EVENT_ABORT_UPGRADE,
    SM_BOOT_EVENT_NUMBER
};

typedef int32_t (*sm_boot_event_handle_fn)(void*, void*);

typedef struct sm_boot_event_handle{
    sm_boot_event_handle_fn m_handle;
    void* m_arg;
}sm_boot_event_handle_t;

typedef struct sm_boot_if_proc{
    int32_t (*init)(sm_boot_if_t*);
    int32_t (*free)(sm_boot_if_t*);
    int32_t (*set_enable)(sm_boot_if_t*, uint8_t);
    int32_t (*set_ready)(sm_boot_if_t*, uint8_t);
    int32_t (*set_fw_valid)(sm_boot_if_t*, uint8_t);
    int32_t (*reg_event_callback)(sm_boot_if_t*, uint8_t, sm_boot_event_handle_fn, void*);
    int32_t (*process)(sm_boot_if_t*);
}sm_boot_if_proc_t;

struct sm_boot_if{
    const sm_boot_if_proc_t* m_proc;
};

#ifdef __cplusplus
};
#endif

#endif //SELEX_HOST_SM_BOOT_IF_H
