//
// Created by vuonglk on 22/03/2023.
//
#include <stdlib.h>

#include "sm_bootloader.h"
#include "sm_memory.h"
#include "sm_timer.h"
#include "sm_logger.h"

#define SM_BOOT_INTERFACE_MAX_SUPPORT   10
#define SM_BOOT_TIME_MIN        0    //sec
#define SM_BOOT_TIME_MAX        10  //sec
#define SM_BOOT_TIME_DEFAULT    2   //sec

#define SM_BOOT_TIME_UPGRADING_MAX  30 // sec

#define SM_BOOT_MAX_RETRY_TIME      300 //sec

#define SM_BOOT_MAX_RETRY           (SM_BOOT_MAX_RETRY_TIME / SM_BOOT_TIME_UPGRADING_MAX)

#define _impl(obj)  ((sm_bootloader*)(obj))

#define TAG "sm_bootloader"

typedef enum {
    SM_BOOTLOADER_STT_INIT,
    SM_BOOTLOADER_STT_WAITING_CONDITION_BOOT,
    SM_BOOTLOADER_STT_PREPARING,
    SM_BOOTLOADER_STT_UPGRADING,
    SM_BOOTLOADER_STT_FINISH,
    SM_BOOTLOADER_STT_FAILURE,
}SM_BOOTLOADER_STT;

typedef enum {
	SM_FIRMWARE_STT_VALID,
	SM_FIRMWARE_STT_INVALID,
}SM_FIRMWARE_STT;

typedef struct sm_bootloader{
    const sm_bootloader_config_t* m_config;
    const sm_bootloader_if_t* m_interface;

    sm_boot_if_t* m_boot_if[SM_BOOT_INTERFACE_MAX_SUPPORT];
    sm_boot_if_t* m_boot_if_active;

    uint8_t m_boot_if_number;
    uint8_t m_boot_time; //second
    sm_elapsed_timer_t m_timeout;
    uint8_t m_retry;

    SM_BOOTLOADER_STT m_state;

    SM_FIRMWARE_STT m_main_app_firmware_status;
    SM_FIRMWARE_STT m_sub_app_firmware_status;

    sm_fw_signature_t m_new_firmware;
    sm_fw_t* m_main_app;
    sm_fw_t* m_sub_app;
}sm_bootloader;

static int32_t sm_bootloader_handle_request_upgrade_event(void* _data, void* _arg);
static int32_t sm_bootloader_handle_new_fw_event(void* _data, void* _arg);
static int32_t sm_bootloader_handle_new_segment_event(void* _data, void* _arg);
static int32_t sm_bootloader_handle_finish_download_event(void* _data, void* _arg);
static int32_t sm_bootloader_handle_finish_upgrade_event(void* _data, void* _arg);
static int32_t sm_bootloader_handle_abort_upgrade_event(void* _data, void* _arg);

static int32_t sm_bootloader_rollback(sm_bootloader_t* _self);
static int32_t sm_bootloader_backup(sm_bootloader_t* _self);
static void    sm_bootloader_init(sm_bootloader_t* _self);
static void    sm_bootloader_preparing(sm_bootloader_t* _self);

static sm_bootloader g_bootloader_default = {
        .m_config = NULL,
        .m_interface = NULL,
        .m_boot_if_number = 0,
        .m_boot_time = SM_BOOT_TIME_MIN,
        .m_boot_if = {NULL,},
        .m_boot_if_active = NULL,
        .m_state = SM_BOOTLOADER_STT_INIT,
        .m_sub_app = NULL,
        .m_main_app = NULL
};

sm_bootloader_t* sm_bootloader_create(const sm_bootloader_config_t* _config,
                                      const sm_bootloader_if_t* _if,
                                      sm_fw_t* _main_app,
                                      sm_fw_t* _sub_app){
    int index = 0;
    sm_bootloader* obj = NULL;

    if(!_config || !_if || !_main_app){
        LOG_ERR(TAG, "Create bootloader FAILURE, missing arguments");
        return NULL;
    }

    obj = (sm_bootloader*)mem_alloc(sizeof(sm_bootloader));
    if(!obj){
        LOG_ERR(TAG, "Create bootloader FAILURE, Allocated memory FAILURE");
        return NULL;
    }

    obj->m_config = _config;
    obj->m_interface = _if;
    obj->m_main_app = _main_app;
    obj->m_sub_app = _sub_app;
    for(index = 0; index < SM_BOOT_INTERFACE_MAX_SUPPORT; index++){
        obj->m_boot_if[index] = NULL;
    }
    obj->m_boot_if_active = NULL;
    obj->m_boot_if_number = 0;
    obj->m_boot_time = SM_BOOT_TIME_MIN;
    obj->m_timeout.m_duration = 0;
    obj->m_timeout.m_start_time = 0;
    obj->m_state = SM_BOOTLOADER_STT_INIT;

    LOG_INF(TAG, "Create bootloader SUCCESS");

    return obj;
}

sm_bootloader_t* sm_bootloader_create_default(const sm_bootloader_config_t* _config,
                                              const sm_bootloader_if_t* _if,
                                              sm_fw_t* _main_app,
                                              sm_fw_t* _sub_app){
    int index;
    if(!_config || !_if || !_main_app){
        LOG_ERR(TAG, "Create bootloader FAILURE, missing arguments");
        return NULL;
    }
    g_bootloader_default.m_config = _config;
    g_bootloader_default.m_interface = _if;
    g_bootloader_default.m_main_app = _main_app;
    g_bootloader_default.m_sub_app = _sub_app;

    for(index = 0; index < SM_BOOT_INTERFACE_MAX_SUPPORT; index++){
        g_bootloader_default.m_boot_if[index] = NULL;
    }
    g_bootloader_default.m_boot_if_active = NULL;

    g_bootloader_default.m_boot_if_number = 0;
    g_bootloader_default.m_boot_if_number = SM_BOOT_TIME_MIN;
    g_bootloader_default.m_boot_time = SM_BOOT_TIME_MIN;
    g_bootloader_default.m_timeout.m_duration = 0;
    g_bootloader_default.m_timeout.m_start_time = 0;
    g_bootloader_default.m_state = SM_BOOTLOADER_STT_INIT;

    LOG_INF(TAG, "Create bootloader default SUCCESS");

    return &g_bootloader_default;
}

int32_t sm_bootloader_destroy(sm_bootloader_t* _self){
    if(!_self){
        return -1;
    }
    mem_free(_self);
    return 0;
}

int32_t sm_bootloader_add_boot_if(sm_bootloader_t* _self, sm_boot_if_t* _boot_if){
    int32_t index = 0;
    if(!_self || !_boot_if || _impl(_self)->m_boot_if_number >= SM_BOOT_INTERFACE_MAX_SUPPORT){
        return -1;
    }
    for(index = 0; index < SM_BOOT_INTERFACE_MAX_SUPPORT; index++){
        if(!_impl(_self)->m_boot_if[index]){
            _impl(_self)->m_boot_if[index] = _boot_if;

            _boot_if->m_proc->reg_event_callback(_boot_if,SM_BOOT_EVENT_REQUEST_UPGRADE, sm_bootloader_handle_request_upgrade_event, _self);
            _boot_if->m_proc->reg_event_callback(_boot_if,SM_BOOT_EVENT_NEW_FIRMWARE, sm_bootloader_handle_new_fw_event, _self);
            _boot_if->m_proc->reg_event_callback(_boot_if,SM_BOOT_EVENT_NEW_SEGMENT, sm_bootloader_handle_new_segment_event, _self);
            _boot_if->m_proc->reg_event_callback(_boot_if,SM_BOOT_EVENT_FINISH_DOWNLOAD, sm_bootloader_handle_finish_download_event, _self);
            _boot_if->m_proc->reg_event_callback(_boot_if,SM_BOOT_EVENT_FINISH_UPGRADE, sm_bootloader_handle_finish_upgrade_event, _self);
            _boot_if->m_proc->reg_event_callback(_boot_if,SM_BOOT_EVENT_ABORT_UPGRADE, sm_bootloader_handle_abort_upgrade_event, _self);

            if(_impl(_self)->m_boot_if[index]->m_proc->set_enable){
                _boot_if->m_proc->set_enable(_boot_if, SM_BOOT_IF_DISABLE);
            }

            _impl(_self)->m_boot_if_number++;

            LOG_DBG(TAG, "Bootloader add interface SUCCESS");
            return 0;
        }
    }
    return -1;
}

int32_t sm_bootloader_reboot(sm_bootloader_t* _self){
    if(!_self){
        return -1;
    }
    LOG_WRN(TAG, "Bootloader reboot NOW");
    _impl(_self)->m_interface->reboot();
}

int32_t sm_bootloader_jump_to_app(sm_bootloader_t* _self){
    if(!_self){
        return -1;
    }
    sm_fw_signature_t signature;
    _impl(_self)->m_config->get_main_app_info(&signature);
    _impl(_self)->m_interface->jump((int32_t)signature.m_addr);

    LOG_INF(TAG, "Bootloader jump to Main Application now");
    return 0;
}

static void sm_bootloader_init(sm_bootloader_t* _self){
    int8_t index;
    sm_fw_signature_t signature;
    signature.m_is_firt_time_update = 0;

    LOG_DBG(TAG, "Bootloader at the initial state, check fw info");

    _impl(_self)->m_config->get_main_app_info(&signature);
    sm_fw_set_signature(_impl(_self)->m_main_app, &signature);

    if(_impl(_self)->m_sub_app){
        _impl(_self)->m_config->get_sub_app_info(&signature);
        sm_fw_set_signature(_impl(_self)->m_sub_app, &signature);
    }

    /// Compatible with old version use flag;
    if(_impl(_self)->m_config->get_flag()){
        _impl(_self)->m_state = SM_BOOTLOADER_STT_PREPARING;
        LOG_WRN(TAG, "The flag is set, compatible with old version");
        goto exit;
    }

    /// Check main porting valid
    if((!sm_fw_is_valid( _impl(_self)->m_main_app))){
    	_impl(_self)->m_main_app_firmware_status = SM_FIRMWARE_STT_INVALID;
        LOG_WRN(TAG, "Main Application INVALID, check SubApp if they are available");
        if(!_impl(_self)->m_sub_app){
            LOG_WRN(TAG, "Sub Application was NOT exits!!!");
            goto exit;
        }
        if(sm_fw_is_valid(_impl(_self)->m_sub_app)){
            sm_fw_format(_impl(_self)->m_sub_app);
            LOG_WRN(TAG, "Sub Application INVALID, format Sub Application");
            goto exit;
        }
        sm_bootloader_rollback(_self);
    }else{
    	_impl(_self)->m_main_app_firmware_status = SM_FIRMWARE_STT_VALID;
        LOG_INF(TAG, "Main Application VALID!!!");
        sm_fw_format(_impl(_self)->m_sub_app);
        goto exit;
    }

exit:
    /// check Boot Time
    _impl(_self)->m_boot_time = _impl(_self)->m_config->get_boot_time();
    if (_impl(_self)->m_boot_time > SM_BOOT_TIME_MAX) {
        _impl(_self)->m_boot_time = SM_BOOT_TIME_DEFAULT;
        _impl(_self)->m_config->set_boot_time(SM_BOOT_TIME_DEFAULT);
    }
    LOG_INF(TAG, "Bootloader boot_core time is : %d sec", _impl(_self)->m_boot_time);

    for (index = 0; index < SM_BOOT_INTERFACE_MAX_SUPPORT; index++) {
        if (_impl(_self)->m_boot_if[index]) {
            _impl(_self)->m_boot_if[index]->m_proc->set_enable(_impl(_self)->m_boot_if[index], SM_BOOT_IF_ENABLE);
            _impl(_self)->m_boot_if[index]->m_proc->init(_impl(_self)->m_boot_if[index]);
        }
    }
    sm_elapsed_timer_resetz(&_impl(_self)->m_timeout, _impl(_self)->m_boot_time * 1000);
    _impl(_self)->m_state = SM_BOOTLOADER_STT_WAITING_CONDITION_BOOT;
    LOG_INF(TAG, "Bootloader switch to Waiting condition BOOT");
}

static void sm_bootloader_preparing(sm_bootloader_t* _self){
    if(!sm_bootloader_backup(_self)){
        _impl(_self)->m_state = SM_BOOTLOADER_STT_FAILURE;
    }
    sm_fw_format(_impl(_self)->m_main_app);

    sm_fw_signature_t signature_current;
    _impl(_self)->m_config->get_main_app_info(&signature_current);
    _impl(_self)->m_config->set_main_app_info(&signature_current);

    if(_impl(_self)->m_boot_if_active && _impl(_self)->m_boot_if_active->m_proc->set_ready){
        _impl(_self)->m_boot_if_active->m_proc->set_ready(_impl(_self)->m_boot_if_active, SM_BOOT_IF_READY);
    }
    _impl(_self)->m_state = SM_BOOTLOADER_STT_UPGRADING;
    _impl(_self)->m_retry = 0;
    sm_elapsed_timer_resetz(&_impl(_self)->m_timeout, SM_BOOT_TIME_UPGRADING_MAX*1000);

    LOG_INF(TAG, "Bootloader switch to upgrading state");
}

static int32_t sm_bootloader_rollback(sm_bootloader_t* _self){
    const sm_fw_signature_t* signature = NULL;

    LOG_WRN(TAG, "Bootloader start rollback backup version progress");

    LOG_DBG(TAG, "Format Main Application partition");
    sm_fw_format(_impl(_self)->m_main_app);

    sm_fw_signature_t signature_current;
    _impl(_self)->m_config->get_main_app_info(&signature_current);
    _impl(_self)->m_config->set_main_app_info(&signature_current);

    if(sm_fw_clone(_impl(_self)->m_main_app, _impl(_self)->m_sub_app) < 0){
        LOG_ERR(TAG, "Clone from Sub Application FAILURE, Rollback fw FAILURE");
        return -1;
    }
    signature = sm_fw_get_signature(_impl(_self)->m_sub_app);
    sm_fw_set_signature(_impl(_self)->m_main_app, signature);
    if(_impl(_self)->m_config->set_main_app_info(signature) < 0){
        LOG_ERR(TAG, "Write main application signature FAILURE, Rollback fw FAILURE");
        return -1;
    }

    LOG_INF(TAG, "Rollback backup version SUCCESS");
    return 0;
}

static int32_t sm_bootloader_backup(sm_bootloader_t* _self){
    const sm_fw_signature_t* signature = NULL;

    LOG_INF(TAG, "Backup current version into Sub Application, Preparing upgrading new fw");

    sm_fw_format(_impl(_self)->m_sub_app);


    if(sm_fw_clone(_impl(_self)->m_sub_app, _impl(_self)->m_main_app) < 0){
        LOG_ERR(TAG, "Clone from main Application FAILURE, Backup fw FAILURE");
        return -1;
    }
    signature = sm_fw_get_signature(_impl(_self)->m_main_app);
    sm_fw_set_signature(_impl(_self)->m_sub_app, signature);
    if(_impl(_self)->m_config->set_sub_app_info(signature) < 0){
        LOG_ERR(TAG, "Write sub application signature FAILURE, Backup fw FAILURE");
        return -1;
    }

    LOG_INF(TAG, "Backup current version SUCCESS");
    return 0;
}

int32_t sm_bootloader_process(sm_bootloader_t* _self){
    int32_t index;

    switch (_impl(_self)->m_state) {
        case SM_BOOTLOADER_STT_INIT:
            sm_bootloader_init(_self);
            break;
        case SM_BOOTLOADER_STT_PREPARING:
            sm_bootloader_preparing(_self);
            break;
        case SM_BOOTLOADER_STT_WAITING_CONDITION_BOOT:
            for(index = 0; index < SM_BOOT_INTERFACE_MAX_SUPPORT; index++){
                if(_impl(_self)->m_boot_if[index] && _impl(_self)->m_boot_if[index]->m_proc->process){
                    _impl(_self)->m_boot_if_active = _impl(_self)->m_boot_if[index];
                    _impl(_self)->m_boot_if_active->m_proc->process(_impl(_self)->m_boot_if_active);
                }
            }
            if(!sm_elapsed_timer_get_remain(&_impl(_self)->m_timeout)){
            	if(_impl(_self)->m_main_app_firmware_status == SM_FIRMWARE_STT_VALID){
            		LOG_INF(TAG, "Main app is available, jump to app now");
                    _impl(_self)->m_state = SM_BOOTLOADER_STT_FINISH;
            	}else{
            		_impl(_self)->m_retry++;
            		if(_impl(_self)->m_retry >= SM_BOOT_MAX_RETRY){
                        _impl(_self)->m_interface->reboot();
            		}
                    LOG_INF(TAG, "Main app is not available, reset ota time out");
                	sm_elapsed_timer_reset(&_impl(_self)->m_timeout);
            	}
            }
            break;
        case SM_BOOTLOADER_STT_UPGRADING:
            _impl(_self)->m_boot_if_active->m_proc->process(_impl(_self)->m_boot_if_active);
            if(!sm_elapsed_timer_get_remain(&_impl(_self)->m_timeout)){
                _impl(_self)->m_state = SM_BOOTLOADER_STT_FAILURE;
            }
            break;
        case SM_BOOTLOADER_STT_FINISH:
            if(!_impl(_self)->m_interface->jump){
                _impl(_self)->m_state = SM_BOOTLOADER_STT_FAILURE;
            }
            _impl(_self)->m_interface->jump(_impl(_self)->m_new_firmware.m_addr);
            break;
        case SM_BOOTLOADER_STT_FAILURE:
            _impl(_self)->m_boot_if_active->m_proc->set_fw_valid(_impl(_self)->m_boot_if_active, SM_BOOT_IF_FW_FAILURE);
            _impl(_self)->m_interface->reboot();
            break;
    }
    return 0;
}

static int32_t sm_bootloader_handle_request_upgrade_event(void* _data, void* _arg){
    int index;
    if(_impl(_arg)->m_state != SM_BOOTLOADER_STT_WAITING_CONDITION_BOOT){
        return -1;
    }
    LOG_WRN(TAG, "Have a upgrading request from system");

    (void*)_data;
    for(index = 0; index < SM_BOOT_INTERFACE_MAX_SUPPORT; index++){
        if(_impl(_arg)->m_boot_if[index]){
            _impl(_arg)->m_boot_if[index]->m_proc->set_enable(_impl(_arg)->m_boot_if[index], SM_BOOT_IF_DISABLE);
        }
    }
    if(_impl(_arg)->m_boot_if_active)
        _impl(_arg)->m_boot_if_active->m_proc->set_enable(_impl(_arg)->m_boot_if_active, SM_BOOT_IF_ENABLE);

    _impl(_arg)->m_state = SM_BOOTLOADER_STT_PREPARING;
    return 0;
}

static int32_t sm_bootloader_handle_new_fw_event(void* _data, void* _arg){
    if(_impl(_arg)->m_state != SM_BOOTLOADER_STT_UPGRADING){
        return -1;
    }

    sm_fw_signature_t* fw_sign = (sm_fw_signature_t*)(_data);
    sm_fw_signature_clone(&_impl(_arg)->m_new_firmware, fw_sign);
    sm_fw_set_signature(_impl(_arg)->m_main_app, fw_sign);
    sm_elapsed_timer_reset(&_impl(_arg)->m_timeout);

    LOG_DBG(TAG, "This is new fw version: \n New Version: %d.%d.%d\n Address: 0x%02X\n Size: %d\n CRC: 0x%02X",
            fw_sign->m_version[0],
            fw_sign->m_version[1],
            fw_sign->m_version[2],
            fw_sign->m_addr,
            fw_sign->m_size,
            fw_sign->m_crc);

    return 0;
}

static int32_t sm_bootloader_handle_new_segment_event(void* _data, void* _arg){
    sm_fw_t* main_app = NULL;
    const sm_segment_t* segment = (const sm_segment_t*)(_data);
    if(_impl(_arg)->m_state != SM_BOOTLOADER_STT_UPGRADING) {
        return -1;
    }
    main_app = _impl(_arg)->m_main_app;
    sm_elapsed_timer_reset(&_impl(_arg)->m_timeout);

    LOG_DBG(TAG, "New segment incoming");

    return sm_fw_write_segment(main_app, segment);
}

static int32_t sm_bootloader_handle_finish_download_event(void* _data, void* _arg){
    if(_impl(_arg)->m_state != SM_BOOTLOADER_STT_UPGRADING){
        _impl(_arg)->m_state = SM_BOOTLOADER_STT_FAILURE;
        return -1;
    }
    sm_fw_signature_t* fw_sign = (sm_fw_signature_t*)(_data);
    sm_fw_set_signature(_impl(_arg)->m_main_app, fw_sign);

    if(!sm_fw_is_valid(_impl(_arg)->m_main_app)){
        _impl(_arg)->m_state = SM_BOOTLOADER_STT_FAILURE;
        LOG_ERR(TAG, "New fw is INVALID, Please try Again");
        return -1;
    }

    _impl(_arg)->m_boot_if_active->m_proc->set_fw_valid(_impl(_arg)->m_boot_if_active, SM_BOOT_IF_FW_SUCCESS);

    return 0;
}

static int32_t sm_bootloader_handle_finish_upgrade_event(void* _data, void* _arg){
    int32_t ret = -1;
    int8_t count = 0;

    (void*)_data;
    if(_impl(_arg)->m_state != SM_BOOTLOADER_STT_UPGRADING){
        _impl(_arg)->m_state = SM_BOOTLOADER_STT_FAILURE;
        return -1;
    }

    while (count++ < 5){
        ret = _impl(_arg)->m_config->set_main_app_info(&_impl(_arg)->m_new_firmware);
        if(ret < 0){
            continue;
        }
    }
    if(ret < 0){
        _impl(_arg)->m_state = SM_BOOTLOADER_STT_FAILURE;
        LOG_ERR(TAG, "Update new configuration for new fw FAILURE");
        return ret;
    }

    _impl(_arg)->m_state = SM_BOOTLOADER_STT_FINISH;
    sm_elapsed_timer_reset(&_impl(_arg)->m_timeout);

    LOG_INF(TAG, "Finish upgrading fw progress, new fw VALID, preparing jump to new Main Application");

    return 0;
}

static int32_t sm_bootloader_handle_abort_upgrade_event(void* _data, void* _arg){
    _impl(_arg)->m_state = SM_BOOTLOADER_STT_FAILURE;

    LOG_ERR(TAG, "Upgrading progress is ABORT, Please check with Admin Boot-Master");
    return 0;
}
