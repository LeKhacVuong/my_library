//
// Created by vuonglk on 27/03/2023.
//
#include <stdio.h>
#include "sm_desktop_board.h"
#include "sm_logger.h"
#include "CRC.h"

#define TAG "sm_desktop_spi_flash_simulation"

#define FILE_NEW_FW_PATH    "./CMakeCache.txt"

static int32_t g_total_seg = 0;
static int32_t g_current_seg = 0;
static char* g_fw_buffer = NULL;

static uint8_t g_flag = 0;

static sm_fw_signature_t g_new_app_signature = {
        .m_addr = 0x00,
        .m_size = 1000,
        .m_is_valid = 0,
        .m_crc = 0x1122,
        .m_version = {0x01, 0x00, 0x01}
};

int32_t spi_flash_init(){
    int32_t file_size;
    FILE* file = fopen(FILE_NEW_FW_PATH, "r");
    if(!file){
        return -1;
    }
    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);

    fseek(file, 0L, SEEK_SET);

    g_fw_buffer = malloc(file_size * sizeof(char));

    fread(g_fw_buffer, sizeof(char), file_size, file);
    fclose(file);

    uint16_t crc = CRC_CalculateCRC16((uint8_t*)g_fw_buffer, file_size);

    g_new_app_signature.m_is_valid = 0;
    g_new_app_signature.m_size = file_size;
    g_new_app_signature.m_addr = 0x00;
    g_new_app_signature.m_crc = crc;

    g_total_seg = file_size/1024;
    if(file_size%1024 != 0){
        g_total_seg += 1;
    }
    g_current_seg = 0;

    return 0;
}

int32_t spi_flash_free(){
    if(g_fw_buffer){
        mem_free(g_fw_buffer);
    }
    return 0;
}

int32_t spi_flash_check_flag(){
    return g_flag;
}

int32_t spi_flash_reset_flag(){
    g_flag = 0;
    return 0;
}
int32_t spi_flash_get_new_fw_info(sm_fw_signature_t* _fw_sign){
    sm_fw_signature_clone(_fw_sign, &g_new_app_signature);
    return 0;
}
int32_t spi_flash_read_segment(sm_segment_t* _seg){
    if(g_fw_buffer){
        int len = 1024;
        uint16_t crc = 0;
        if(g_current_seg >= (g_total_seg - 1)){
            len = g_new_app_signature.m_size - g_current_seg*1024;
            sm_seg_set_last_segment(_seg, 1);
        }else{
            len = 1024;
            sm_seg_set_last_segment(_seg, 0);
        }
        crc = CRC_CalculateCRC16((uint8_t*)g_fw_buffer + g_current_seg*1024, len);
        sm_seg_set_info(_seg, g_current_seg, g_current_seg, len, crc);

        sm_seg_fill_data(_seg, (uint8_t*)(g_fw_buffer + g_current_seg*1024), len);

        g_current_seg++;
        return 0;
    }
    return -1;
}

static sm_spi_flash_if_t g_spi_flash_default = {
    .init = spi_flash_init,
    .free = spi_flash_free,
    .check_flag = spi_flash_check_flag,
    .reset_flag = spi_flash_reset_flag,
    .get_new_fw_info = spi_flash_get_new_fw_info,
    .read_segment = spi_flash_read_segment
};

const sm_spi_flash_if_t* sm_board_get_spi_flash(){
    return &g_spi_flash_default;
}