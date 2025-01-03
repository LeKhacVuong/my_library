//
// Created by vnbk on 22/03/2023.
//

#ifndef SELEX_HOST_SM_FW_H
#define SELEX_HOST_SM_FW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sm_segment.h"
#include "sm_memory.h"

#define SM_FW_VERSION_LENGTH    3

typedef struct sm_fw sm_fw_t;

typedef struct sm_fw_signature{
    uint8_t m_version[SM_FW_VERSION_LENGTH];
    uint32_t m_addr;
    uint32_t m_size;
    uint16_t m_crc;
    uint8_t m_is_valid;
    uint8_t m_is_firt_time_update;
}sm_fw_signature_t;

static inline int32_t sm_fw_signature_clone(sm_fw_signature_t* _self, const sm_fw_signature_t* _other){
    if(!_other){
        return -1;
    }
    sm_memcpy(_self->m_version, _other->m_version, SM_FW_VERSION_LENGTH);
    _self->m_addr = _other->m_addr;
    _self->m_size = _other->m_size;
    _self->m_crc = _other->m_crc;
    _self->m_is_valid = _other->m_is_valid;
    _self->m_is_firt_time_update = _other->m_is_firt_time_update;
    return 0;
}

typedef struct sm_fw_storage_opt{
    int32_t (*init)();
    int32_t (*free)();
    const uint8_t* (*read)(uint32_t);
    int32_t (*write_segment)(uint32_t, const uint8_t*, uint32_t);
    int32_t (*write_block)(uint32_t, const uint8_t*, uint32_t);
    int32_t (*erase_block)(uint32_t);
    int32_t (*erase_full)();
}sm_fw_storage_opt_t;

sm_fw_t* sm_fw_create(const sm_fw_storage_opt_t* _mem_opt);
sm_fw_t* sm_fw_create_default(const sm_fw_storage_opt_t* _mem_opt);
int32_t sm_fw_destroy(sm_fw_t* _self);

int32_t sm_fw_set_signature(sm_fw_t* _self, const sm_fw_signature_t* _sign);
const sm_fw_signature_t* sm_fw_get_signature(sm_fw_t* _self);

int32_t sm_fw_is_valid(sm_fw_t* _self);
int32_t sm_fw_format(sm_fw_t* _self);
int32_t sm_fw_write_segment(sm_fw_t* _self, const sm_segment_t* _segment);
int32_t sm_fw_clone(sm_fw_t* _self, sm_fw_t* _other);

#ifdef __cplusplus
};
#endif

#endif //SELEX_HOST_SM_FW_H
