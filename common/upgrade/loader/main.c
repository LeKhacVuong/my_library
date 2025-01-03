#include <stdio.h>
#include <string.h>

#include "app/board/desktop/sm_desktop_board.h"
#include "host/sm_cmd.h"
#include "host/sm_host.h"
#include "sm_boot_host.h"
#include "sm_boot_canopen.h"
#include "sm_boot_spi_flash.h"
#include "CRC.h"

#include "app_co_init.h"

int main(int argc, char** argv) {
    printf("Hello, World!\n");
    int canopen_node_id = 0;
    char* temp;

    if(argc == 2){
        canopen_node_id = strtol(argv[1], &temp, 10);
    }else{
        canopen_node_id = 4;
    }


    sm_board_init();

    sm_fw_storage_opt_t* mem_opt = sm_board_get_fw_storage_opt();
    sm_bootloader_config_t* config = sm_board_get_bootloader_config();
    sm_bootloader_if_t* bootloader_if = sm_board_get_bootloader_interface();

    sm_fw_t* main_app = sm_fw_create_default(mem_opt);
    sm_fw_t* sub_app = sm_fw_create(mem_opt);

   /* sm_host_t* host = sm_board_get_host();
    sm_boot_if_t* host_boot = sm_boot_host_create_default(host);*/

    sm_boot_if_t* canopen_boot = sm_boot_canopen_create_default(canopen_node_id,3);
//    sm_boot_if_t* spi_flash_boot = sm_boot_spi_flash_create_default(sm_board_get_spi_flash());

    sm_bootloader_t* g_bootloader = sm_bootloader_create_default(config, bootloader_if, main_app, sub_app);
//    sm_bootloader_add_boot_if(g_bootloader,host_boot);
    sm_bootloader_add_boot_if(g_bootloader, canopen_boot);
//    sm_bootloader_add_boot_if(g_bootloader, spi_flash_boot);

//    timeout = get_tick_count() + 5*1000;
    while (1){
        sm_bootloader_process(g_bootloader);
        if(sm_board_process() < 0){
            break;
        }
        app_process();
    }
    printf("Exit Bootloader Program. Say Goodbye and See you!!!\n");
//    while (1);

    return 0;
}
