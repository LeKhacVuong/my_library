//
// Created by vuonglk on 22/03/2023.
//

#ifndef SELEX_BOOTLOADER_H
#define SELEX_BOOTLOADER_H

#ifdef __cplusplus
extern "C"{
#endif

#include "sm_boot_if.h"
#include "sm_fw.h"

typedef void sm_bootloader_t;

typedef struct sm_bootloader_config{
    int32_t (*is_valid)();
    int32_t (*get_flag)(); //Compatible old version;
	int32_t (*get_main_app_info)(sm_fw_signature_t*);
    int32_t (*set_main_app_info)(const sm_fw_signature_t*);
    int32_t (*get_sub_app_info)(sm_fw_signature_t*);
    int32_t (*set_sub_app_info)(const sm_fw_signature_t*);
    int32_t (*get_boot_time)();
    int32_t (*set_boot_time)(const int32_t);
    int32_t (*get_option)(void*);
    int32_t (*set_option)(const void*);
}sm_bootloader_config_t;

typedef struct sm_bootloader_if{
    int32_t (*reboot)();
    int32_t (*jump)(int32_t);
}sm_bootloader_if_t;

sm_bootloader_t* sm_bootloader_create(const sm_bootloader_config_t* _config,
                                      const sm_bootloader_if_t* _if,
                                      sm_fw_t* _main_app,
                                      sm_fw_t* _sub_app);
sm_bootloader_t* sm_bootloader_create_default(const sm_bootloader_config_t* _config,
                                              const sm_bootloader_if_t* _if,
                                              sm_fw_t* _main_app,
                                              sm_fw_t* _sub_app);
int32_t sm_bootloader_destroy(sm_bootloader_t* self);

int32_t sm_bootloader_add_boot_if(sm_bootloader_t* self, sm_boot_if_t* _boot_if);

int32_t sm_bootloader_reboot(sm_bootloader_t* self);
int32_t sm_bootloader_jump_to_app(sm_bootloader_t* self);

int32_t sm_bootloader_process(sm_bootloader_t* self);

#ifdef __cplusplus
};
#endif

#endif //SELEX_BOOTLOADER_H
