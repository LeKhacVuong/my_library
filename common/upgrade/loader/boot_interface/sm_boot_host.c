//
// Created by vuonglk on 22/03/2023.
//
#include <stdlib.h>

#include "sm_boot_host.h"
#include "sm_cmd.h"
#include "sm_fw.h"
#include "sm_memory.h"
#include "sm_logger.h"

#define TAG "sm_boot_host"

#define _imp(p) ((sm_boot_host_impl_t*)(p))

typedef struct{
    sm_boot_if_t m_base;
    sm_host_t* m_host;

    sm_boot_event_handle_t m_handles[SM_BOOT_EVENT_NUMBER];

    sm_fw_signature_t m_fw_signature;
    sm_segment_t* m_segment;

    uint8_t m_enable;
}sm_boot_host_impl_t;

static int32_t sm_boot_host_init(sm_boot_if_t*);
static int32_t sm_boot_host_free(sm_boot_if_t*);
static int32_t sm_boot_host_enable(sm_boot_if_t*, uint8_t);
static int32_t sm_boot_host_set_ready(sm_boot_if_t*, uint8_t);
static int32_t sm_boot_set_fw_valid(sm_boot_if_t*, uint8_t);
static int32_t sm_boot_host_reg_event_callback(sm_boot_if_t*,  uint8_t, sm_boot_event_handle_fn, void*);
static int32_t sm_boot_host_process(sm_boot_if_t*);

static int32_t sm_boot_cmd_handle(int32_t, const uint8_t*, int32_t, void*);

static const sm_boot_if_proc_t g_boot_if_proc_default = {
        .init = sm_boot_host_init,
        .free = sm_boot_host_free,
        .set_enable = sm_boot_host_enable,
        .reg_event_callback = sm_boot_host_reg_event_callback,
        .set_ready = sm_boot_host_set_ready,
        .set_fw_valid = sm_boot_set_fw_valid,
        .process = sm_boot_host_process
};

static sm_boot_host_impl_t g_boot_host_default = {
        .m_host = NULL,
        .m_segment = NULL,
        .m_enable = SM_BOOT_IF_DISABLE
};

sm_boot_if_t* sm_boot_host_create(sm_host_t* _host){
    int index;
    if(!_host){
        return NULL;
    }
    sm_boot_host_impl_t* obj = mem_alloc(sizeof(sm_boot_host_impl_t));
    if(!obj){
        return NULL;
    }
    obj->m_base.m_proc = &g_boot_if_proc_default;
    obj->m_host = _host;
    for(index = 0; index < SM_BOOT_EVENT_NUMBER; index++){
        obj->m_handles->m_arg = NULL;
        obj->m_handles->m_handle = NULL;
    }
    obj->m_enable = SM_BOOT_IF_DISABLE;
    obj->m_segment = sm_seg_create();

    return &obj->m_base;
}

sm_boot_if_t* sm_boot_host_create_default(sm_host_t* _host){
    int index;
    if(!_host){
        return NULL;
    }
    g_boot_host_default.m_base.m_proc = &g_boot_if_proc_default;
    g_boot_host_default.m_host = _host;
    for(index = 0; index < SM_BOOT_EVENT_NUMBER; index++){
        g_boot_host_default.m_handles->m_arg = NULL;
        g_boot_host_default.m_handles->m_handle = NULL;
    }
    g_boot_host_default.m_enable = SM_BOOT_IF_DISABLE;
    g_boot_host_default.m_segment = sm_seg_create_default();

    return &g_boot_host_default.m_base;
}

static int32_t sm_boot_host_init(sm_boot_if_t* _self){
    if(!_self){
        return -1;
    }
    sm_host_reg_handle(_imp(_self)->m_host, sm_boot_cmd_handle, _self);
    return 0;
}
static int32_t sm_boot_host_free(sm_boot_if_t* _self){
    if(!_self){
        return -1;
    }
    return 0;
}

static int32_t sm_boot_host_enable(sm_boot_if_t* _self, uint8_t _is_enable){
    if(!_self){
        return -1;
    }
    _imp(_self)->m_enable = _is_enable;
    return 0;
}

static int32_t sm_boot_host_set_ready(sm_boot_if_t* _self, uint8_t _is_busy){
    if(!_self){
        return -1;
    }
    return 0;
}

static int32_t sm_boot_set_fw_valid(sm_boot_if_t* _self, uint8_t _is_success){
    if(!_self){
        return -1;
    }
    return 0;
}

static int32_t sm_boot_host_reg_event_callback(sm_boot_if_t* _self, uint8_t _event,  sm_boot_event_handle_fn _fn_callback, void* _arg){
    if(!_self || !_fn_callback || _event >= SM_BOOT_EVENT_NUMBER){
        return -1;
    }
    _imp(_self)->m_handles[_event].m_handle = _fn_callback;
    _imp(_self)->m_handles[_event].m_arg = _arg;
    return 0;
}
static int32_t sm_boot_host_process(sm_boot_if_t* _self){
    if(!_self){
        return -1;
    }
    if(!_imp(_self)->m_enable){
        return -1;
    }
    sm_host_process(_imp(_self)->m_host);
    return 0;
}

static int32_t sm_boot_cmd_handle(int32_t _cmd, const uint8_t* _data, int32_t _data_len, void* _arg){
    sm_boot_host_impl_t* boot_host = _imp(_arg);
    uint8_t sub_cmd;
    uint8_t index = 0;
    uint8_t last_seg = 0;
    uint32_t addr = 0, length = 0;
    uint16_t crc = 0;
    int32_t ret = 0;

    if(_cmd != SM_CMD_UPGRADE_FW || _data_len < 1){
        return -1;
    }
    sub_cmd = _data[0];

    switch (sub_cmd) {
        case SM_SUB_CMD_REQUEST:
            LOG_DBG(TAG, "Handle request upgrading fw from system");
            if(boot_host->m_handles[SM_BOOT_EVENT_REQUEST_UPGRADE].m_handle){
                ret = boot_host->m_handles[SM_BOOT_EVENT_REQUEST_UPGRADE].m_handle(NULL, boot_host->m_handles[SM_BOOT_EVENT_REQUEST_UPGRADE].m_arg);
            }
            break;
        case SM_SUB_CMD_FW_INFO:
            if(_data_len < 14) {
                goto abort;
            }
            sm_memcpy(boot_host->m_fw_signature.m_version, &_data[1], SM_FW_VERSION_LENGTH);
            boot_host->m_fw_signature.m_size = sm_getUint32(&_data[4]);
            boot_host->m_fw_signature.m_addr = sm_getUint32(&_data[8]);
            boot_host->m_fw_signature.m_crc = sm_getUint16(&_data[12]);
            if(boot_host->m_handles[SM_BOOT_EVENT_NEW_FIRMWARE].m_handle){
                ret = boot_host->m_handles[SM_BOOT_EVENT_NEW_FIRMWARE].m_handle(&boot_host->m_fw_signature,
                                                                                boot_host->m_handles[SM_BOOT_EVENT_NEW_FIRMWARE].m_arg);
            }

            if(boot_host->m_handles[SM_BOOT_EVENT_FINISH_DOWNLOAD].m_handle){
                ret = boot_host->m_handles[SM_BOOT_EVENT_FINISH_DOWNLOAD].m_handle(&boot_host->m_fw_signature,
                                                                                   boot_host->m_handles[SM_BOOT_EVENT_FINISH_DOWNLOAD].m_arg);
                if(ret >= 0){
                    if(boot_host->m_handles[SM_BOOT_EVENT_FINISH_UPGRADE].m_handle){
                        ret = boot_host->m_handles[SM_BOOT_EVENT_FINISH_UPGRADE].m_handle(&boot_host->m_fw_signature,
                                                                                          boot_host->m_handles[SM_BOOT_EVENT_FINISH_UPGRADE].m_arg);
                    }
                }
            }

            break;
        case SM_SUB_CMD_SEND_SEGMENT_INFO:
            if(_data_len < 12)
                goto abort;

            index = _data[1];
            last_seg = _data[2];
            addr = sm_getUint32(&_data[3]);
            length = sm_getUint32(&_data[7]);
            crc = sm_getUint16(&_data[11]);

            LOG_INF(TAG, "Handle segment info: address: 0x%02X, length: %d, crc: 0x%02X, last segment: %d", addr, length, crc, last_seg);

            sm_seg_set_info(boot_host->m_segment, index, (int32_t)addr, (int32_t)length, crc);
            sm_seg_set_last_segment(boot_host->m_segment, last_seg);
            break;
        case SM_SUB_CMD_SEND_SEGMENT_DATA:
            sm_seg_fill_data(boot_host->m_segment, &_data[1], _data_len - 1);
            if(sm_seg_is_full(boot_host->m_segment) && sm_seg_is_valid(boot_host->m_segment)){
                if(boot_host->m_handles[SM_BOOT_EVENT_NEW_SEGMENT].m_handle){
                    LOG_INF(TAG, "New segment data is received fully");
                    ret = boot_host->m_handles[SM_BOOT_EVENT_NEW_SEGMENT].m_handle(boot_host->m_segment,
                                                                                   boot_host->m_handles[SM_BOOT_EVENT_NEW_SEGMENT].m_arg);
                }
            }
            break;
        default:
            break;
    }
    ret = (ret >= 0) ? SM_RES_NONE : -1;
    if(ret < 0){
        LOG_ERR(TAG, "Ret < 0");
    }
    sm_host_send_response(boot_host->m_host,_cmd, (int8_t)ret, &sub_cmd, 1);

    return 0;

abort:
    if(boot_host->m_handles[SM_BOOT_EVENT_ABORT_UPGRADE].m_handle){
        boot_host->m_handles[SM_BOOT_EVENT_ABORT_UPGRADE].m_handle(NULL, boot_host->m_handles[SM_BOOT_EVENT_ABORT_UPGRADE].m_arg);
    }
    sm_host_send_response(boot_host->m_host,_cmd, SM_RES_ERR_COMMON, NULL, 0);
    return -1;
}
