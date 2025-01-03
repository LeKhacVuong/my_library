//
// Created by vuonglk on 23/03/2023.
//
#include <stdio.h>

#include "sm_desktop_board.h"
#include "CRC.h"
#include "sm_timer.h"
#include "sm_logger.h"
#include "linux_serial.h"

#ifdef __linux
#include <unistd.h>
#include <time.h>

#define TAG "sm_desktop_board"

int get_tick_count(){
    struct timespec ts;
    unsigned int tick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    tick  = ts.tv_nsec / 1000000;
    tick += ts.tv_sec * 1000;
    return (int)tick;
}

#define BOARD_FILE_PATH "./CMakeCache.txt"
static FILE* g_file = NULL;
static char* g_content_file = NULL;
static int g_total_seg = 0;
static sm_fw_signature_t g_new_app_signature = {
        .m_version = {0x01, 0x00, 0x02}
};


static sm_host_t* g_host_default = NULL;
static const char* g_serial_port = "/dev/ttyUSB1";
static int32_t g_serial_fd = -1;

static int32_t host_send_impl(const uint8_t* _data, int32_t _len){
    if(g_serial_fd < 0){
        return -1;
    }
    return serial_send_bytes(g_serial_fd, _data, _len);
}

static int32_t host_response(const uint8_t* _data, int32_t _len){

    printf("Data is response: ");
    for (int i = 0; i < _len; ++i) {
        printf("0x%02X-", _data[i]);
    }
    printf("\n");

    return 0;
}

static int32_t host_build_request_upgrade(uint8_t* _packet){
    uint8_t packet[] = {0x00};
    sm_memcpy(_packet, packet, sizeof (packet));
    return sizeof(packet);
}
static int32_t host_build_new_fw_info(uint8_t* packet){
    int len = 0;
//    uint8_t packet[50] = {0x01, 0x00, 0x01, 0x00, 0x01, 0x11, 0x11, 0x00, 0x00, 0x00,0x00};
    packet[len++] = 0x01;
    sm_memcpy(&packet[len], g_new_app_signature.m_version, 3);
    len += 3;
    packet[len++] = (g_new_app_signature.m_size >> 24) & 0xFF;
    packet[len++] = (g_new_app_signature.m_size >> 16) & 0xFF;
    packet[len++] = (g_new_app_signature.m_size >> 8) & 0xFF;
    packet[len++] = (g_new_app_signature.m_size) & 0xFF;

    packet[len++] = (g_new_app_signature.m_addr >> 24) & 0xFF;
    packet[len++] = (g_new_app_signature.m_addr >> 16) & 0xFF;
    packet[len++] = (g_new_app_signature.m_addr >> 8) & 0xFF;
    packet[len++] = (g_new_app_signature.m_addr) & 0xFF;

    packet[len++] = (g_new_app_signature.m_crc >> 8) & 0xFF;
    packet[len++] = g_new_app_signature.m_crc & 0xFF;

    return len;
}

static int g_current_seg = 0;
static int32_t host_build_new_seg_info(uint8_t* packet, int32_t seg_size){
    int32_t len = 0;
    packet[len++] = 0x03;
    packet[len++] = g_current_seg;
    packet[len++] = 0x00;
    packet[len++] = 0x00;
    packet[len++] = 0x00;
    packet[len++] = g_current_seg; // Address Segment

    packet[len++] = (seg_size >> 24) & 0xFF;
    packet[len++] = (seg_size >> 16) & 0xFF;;
    packet[len++] = (seg_size >> 8) & 0xFF;;
    packet[len++] = (seg_size) & 0xFF;; // segment size

    uint16_t crc = CRC_CalculateCRC16(g_content_file + g_current_seg*1024, seg_size);
    packet[len++] = (crc >> 8) & 0xFF;
    packet[len++] = crc & 0xFF;

//    printf("Build Segment Info: current segment: %d\n", g_current_seg);
    return len;
}
static int32_t host_build_seg_data(uint8_t* packet, const uint8_t* data, int32_t length, int32_t offset){
    int len = 0;
    packet[len++] = 0x04;
    sm_memcpy(&packet[len], data + offset, length);
    len += length;
//    printf("Build Segment Data: %d\n", g_current_seg);
    return len;
}
static int32_t host_build_finish_seg(uint8_t* packet){
    int len = 0;
    packet[len++] = 0x05;
    packet[len++] = g_current_seg;
    if(g_current_seg == g_total_seg - 1){
        packet[len++] = 0x01;
    }else{
        packet[len++] = 0x00;
    }

//    printf("Build finish segment]\n");

    return len;
}
static int32_t host_build_finish_fw(uint8_t* packet){
    int len = 0;
    packet[len++] = 0x06;

//    printf("Build finish fw]\n");

    return len;
}

static int32_t host_feed(){
    static int g_step = 0;
    static int g_offset = 0;
    static int g_packet_index = 0;
    static int g_last_seg_size = 0;

    uint8_t packet[1024] = {0x00, 0x05, 0x01, 0x08, };
    uint16_t len = 0;

    if(g_step == 0){
        len = host_build_request_upgrade(&packet[4]);
        g_step++;
    }else if(g_step == 1){
        len = host_build_new_fw_info(&packet[4]);
        g_step++;
    }else if(g_step == 2){
        if(g_current_seg != g_total_seg - 1){
            len = host_build_new_seg_info(&packet[4], 1024);
        }else{
            g_last_seg_size = g_new_app_signature.m_size - g_current_seg*1024;
            len = host_build_new_seg_info(&packet[4], g_last_seg_size);
        }
        g_step++;
    }else if(g_step == 3){
        if(g_current_seg != g_total_seg - 1) {
            len = host_build_seg_data(&packet[4], (uint8_t *) (g_content_file + (g_current_seg * 1024)), 128, g_offset);
            g_offset += 128;
            if (g_offset >= 1024) {
                g_offset = 0;
                g_step++;
            }
        }else{
            len = host_build_seg_data(&packet[4], (uint8_t *) (g_content_file + (g_current_seg * 1024)), g_last_seg_size, 0);
            g_step++;
        }
    }else if(g_step == 4){
        len = host_build_finish_seg(&packet[4]);
        g_current_seg++;
        if(g_current_seg >= g_total_seg){
            g_step++;
            g_current_seg = 0;
        }else{
            g_step -= 2;
        }
    }else if(g_step == 5){
        len = host_build_finish_fw(&packet[4]);
        g_step++;
    }

    len += 4;

    packet[0] = ((len) >> 8) & 0xff;
    packet[1] = ((len) & 0xff);

    uint8_t start = 0xAA;
    uint8_t stop = 0x55;

    uint16_t crc = CRC_CalculateCRC16(packet, len);
    uint8_t crc_array[2] = {(crc >> 8) & 0xFF, crc & 0xFF};

    sm_host_asyn_feed(&start, 1, g_host_default);
    sm_host_asyn_feed(packet, len, g_host_default);
    sm_host_asyn_feed((const uint8_t*)&crc_array, 2, g_host_default);
    sm_host_asyn_feed(&stop, 1, g_host_default);

//    printf("Feed data to host\n");
    return 0;
}


static void sm_board_log_output(const char* data){
    printf("%s\n", data);
}

void sm_board_init(){
    sm_logger_init(sm_board_log_output, LOG_LEVEL_DEBUG);

    g_serial_fd = serial_init(g_serial_port, 115200, !SERIAL_FLAG_BLOCKING);
    if(g_serial_fd < 0){
        LOG_ERR(TAG, "Could not initialized Serial port");
        return;
    }

    g_host_default = sm_host_create_default(1, host_send_impl);

    /*g_file = fopen(BOARD_FILE_PATH, "r");
    if(!g_file){
        return;
    }
    fseek(g_file, 0L, SEEK_END);
    int g_file_length = ftell(g_file);

    fseek(g_file, 0L, SEEK_SET);

    g_content_file = malloc(g_file_length * sizeof(char));

    fread(g_content_file, sizeof(char), g_file_length, g_file);
    fclose(g_file);

    uint16_t crc = CRC_CalculateCRC16((uint8_t*)g_content_file, g_file_length);

    g_new_app_signature.m_is_valid = 0;
    g_new_app_signature.m_size = g_file_length;
    g_new_app_signature.m_addr = 0x00;
    g_new_app_signature.m_crc = crc;

    g_total_seg = g_file_length/1024;
    if(g_file_length%1024 != 0){
        g_total_seg += 1;
    }*/

//    LOG_WRN("Desktop Board", "New fw have size: %d, total segment: %d\n", g_file_length, g_total_seg);
}

static sm_elapsed_timer_t g_timeout = {
        .m_start_time = 0,
        .m_duration = 50,
};
static int g_exit_bootloader = 0;

int32_t sm_board_process(){
    uint8_t buf[256] = {0, };
    int len = serial_recv_bytes(g_serial_fd, buf, 256);
    if(len > 0){
        sm_host_asyn_feed(buf, len, g_host_default);
    }

    if(!sm_elapsed_timer_get_remain(&g_timeout)){
//        host_feed();
        sm_elapsed_timer_reset(&g_timeout);
    }
    if(g_exit_bootloader){
//        free(g_content_file);
        return -1;
    }
    return 0;
}


sm_host_t* sm_board_get_host(){
    return g_host_default;
}


static int32_t board_reboot(){
    LOG_WRN("Desktop Board", "Board Reboot, Please waiting.....\n");
    g_exit_bootloader = 1;
    return 0;
}

int32_t board_jump_app(int32_t _addr){
    LOG_WRN("Desktop Board", "Jump to Main Application, Please waiting.....\n");
    g_exit_bootloader = 1;
    return 0;
}

static sm_bootloader_if_t g_bootloader_if = {
    .jump = board_jump_app,
    .reboot = board_reboot
};

sm_bootloader_if_t* sm_board_get_bootloader_interface(){
    return &g_bootloader_if;
}

#endif
