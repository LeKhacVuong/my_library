//
// Created by vuonglk on 23/03/2023.
//

#ifndef SELEX_BOOTLOADER_SM_DESKTOP_BOARD_H
#define SELEX_BOOTLOADER_SM_DESKTOP_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "sm_fw.h"
#include "sm_bootloader.h"
#include "sm_host.h"
#include "sm_boot_spi_flash.h"

extern int32_t get_tick_count();

void sm_board_init();
int32_t sm_board_process();

sm_fw_storage_opt_t* sm_board_get_fw_storage_opt();
sm_bootloader_config_t* sm_board_get_bootloader_config();
sm_host_t* sm_board_get_host();
sm_bootloader_if_t* sm_board_get_bootloader_interface();

const sm_spi_flash_if_t* sm_board_get_spi_flash();

#ifdef __cplusplus
};
#endif

#endif //SELEX_BOOTLOADER_SM_DESKTOP_BOARD_H
