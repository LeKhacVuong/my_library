//
// Created by vuonglk on 27/03/2023.
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "sm_fw.h"
#include "sm_desktop_board.h"

#define FILE_NEW_FW_PATH    "./HMI.bin"
static char* g_fw_buffer = NULL;

int32_t file_init(){
    FILE* fw_file = fopen(FILE_NEW_FW_PATH, "w");
    fclose(fw_file);
    return 0;
}

int32_t file_free(){
    return 0;
}

const uint8_t* file_read(uint32_t _addr){
    FILE* fw_file = fopen(FILE_NEW_FW_PATH, "r");
    if(fw_file){
        fseek(fw_file, 0L, SEEK_END);
        int file_length = ftell(fw_file);
        if(file_length <= 0){
            fclose(fw_file);
            return NULL;
        }
        fseek(fw_file, 0L, SEEK_SET);

        g_fw_buffer = malloc(file_length * sizeof(char));

        fread(g_fw_buffer, sizeof(char), file_length, fw_file);

        fclose(fw_file);

        return g_fw_buffer;
    }
    return NULL;
}

int32_t file_write_segment(uint32_t _addr, const uint8_t* _data, uint32_t _size){
    FILE* fw_file = fopen(FILE_NEW_FW_PATH, "a");
    if(fw_file < 0){
        return -1;
    }
    int len = fwrite(_data, sizeof(char), _size, fw_file);

    fclose(fw_file);
    return len;
}

int32_t file_write_block(uint32_t _addr, const uint8_t* _data, uint32_t _size){

}

int32_t file_erase_block(uint32_t _addr){

}

int32_t file_erase_full(){
//    file_init();
}

static sm_fw_storage_opt_t g_mem_opt_default = {
        .init = file_init,
        .free = file_free,
        .read = file_read,
        .write_segment = file_write_segment,
        .write_block = file_write_block,
        .erase_block = file_erase_block,
        .erase_full = file_erase_full
};

sm_fw_storage_opt_t* sm_board_get_fw_storage_opt(){
    return &g_mem_opt_default;
}

static sm_fw_signature_t g_main_app_signature = {
        .m_addr = 0x00,
        .m_size = 1000,
        .m_is_valid = 0,
        .m_crc = 0x1122,
        .m_version = {0x01, 0x00, 0x01}
};

static sm_fw_signature_t g_sub_app_signature = {
        .m_addr = 0x00,
        .m_size = 1000,
        .m_is_valid = 0,
        .m_crc = 0x1122,
        .m_version = {0x01, 0x00, 0x01}
};

int32_t is_valid(){
    return 1;
}
int32_t get_flag(){
    return 0;
}
int32_t get_main_app_info(sm_fw_signature_t* _fw_sign){
    sm_fw_signature_clone(_fw_sign, &g_main_app_signature);
    return 0;
}
int32_t set_main_app_info(const sm_fw_signature_t* _fw_sign){
    sm_fw_signature_clone(&g_main_app_signature, _fw_sign);
    return 0;
}
int32_t get_sub_app_info(sm_fw_signature_t* _fw_sign){
    sm_fw_signature_clone(_fw_sign, &g_sub_app_signature);
    return 0;
}
int32_t set_sub_app_info(const sm_fw_signature_t* _fw_sign){
    sm_fw_signature_clone(&g_sub_app_signature, _fw_sign);
    return 0;
}

static int g_boot_time = 9;
int32_t get_boot_time(){
    printf("Get Boot Time\n");
    return g_boot_time;
}
int32_t set_boot_time(const int32_t _boot_time){
    g_boot_time = _boot_time;
    return g_boot_time;
}
int32_t get_option(void* _data){
    return 0;
}
int32_t set_option(const void* _data){
    return 0;
}

static sm_bootloader_config_t g_config_opt_default = {
        .is_valid = is_valid,
        .get_flag = get_flag,
        .get_main_app_info = get_main_app_info,
        .set_main_app_info = set_main_app_info,
        .get_sub_app_info = get_sub_app_info,
        .set_sub_app_info = set_main_app_info,
        .get_boot_time = get_boot_time,
        .set_boot_time = set_boot_time,
        .get_option = get_option,
        .set_option = set_option
};

sm_bootloader_config_t* sm_board_get_bootloader_config(){
    return &g_config_opt_default;
}
