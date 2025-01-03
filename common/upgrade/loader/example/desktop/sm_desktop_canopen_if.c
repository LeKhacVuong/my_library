//
// Created by vuonglk on 29/03/2023.
//
#include <stdio.h>
#include <string.h>

#ifdef __linux
#include <unistd.h>
#endif

#include "sm_desktop_canopen_if.h"
#include "tcp_server.h"

#define SERVER_PORT_DEFAULT                         32000

#define PACKET_LENGTH                               13
#define FRAME_ID_LENGTH                             4
#define FRAME_DATA_LENGTH                           8

#define MAX_PACKET            50
#define MAX_BUFFER            (PACKET_LENGTH)


#define FRAME_INFO_DEFAULT        0x00
#define FRAME_INFO_LENGTH_MASK    0x0F

typedef struct sm_canopen_interface_packet {
    uint8_t m_info;
    uint32_t m_frame_id;
    uint8_t m_data_len;
    uint8_t m_frame_data[FRAME_DATA_LENGTH];
}sm_canopen_interface_packet_t;

struct sm_canopen_if{
    tcp_server_t* m_server;
    tcp_session_t* m_session;
    sm_canopen_if_receive_callback_fn m_callback;
};

static  sm_canopen_if_t g_canopen_if_default = {
        .m_server = NULL,
        .m_session = NULL,
        .m_callback = NULL
};

sm_canopen_if_t* sm_dk_canopen_if_create_default(sm_canopen_if_receive_callback_fn _fn){
    g_canopen_if_default.m_callback = _fn;

    tcp_server_t* server = tcp_server_create_default(SERVER_PORT_DEFAULT);
    if(server){
        g_canopen_if_default.m_server = server;
        return &g_canopen_if_default;
    }
    return NULL;
}

int32_t sm_dk_canopen_if_send(sm_canopen_if_t* _self, uint32_t _cob_id, const uint8_t* data, int32_t _len){
    if(_self->m_session && tcp_session_is_connected(_self->m_session)){
        uint8_t buf[PACKET_LENGTH] = {0,};
        int index = 0;
        buf[index++] = FRAME_INFO_DEFAULT | (_len & FRAME_INFO_LENGTH_MASK);
        buf[index++] = (_cob_id >> 24) & 0xFF;
        buf[index++] = (_cob_id >> 16) & 0xFF;
        buf[index++] = (_cob_id >> 8) & 0xFF;
        buf[index++] = _cob_id & 0xFF;
        for(int i = 0; i < _len; i++){
            buf[index++] = data[i];
        }
        return tcp_session_send(_self->m_session, buf, index, 100);
    }
    return -1;
}

static int sm_canopen_interface_decode_packet(const uint8_t* data, int len, sm_canopen_interface_packet_t* packets){
    if(len < PACKET_LENGTH){
        return -1;
    }
    sm_canopen_interface_packet_t* packet = packets;
    int number = 0;

    for(int i = 0; i < len; i += PACKET_LENGTH){
        memset(packet, 0, sizeof(sm_canopen_interface_packet_t));

        packet->m_info = data[i];
        packet->m_data_len = packet->m_info & FRAME_INFO_LENGTH_MASK;

        packet->m_frame_id |= ((uint32_t)data[i+1] << 24);
        packet->m_frame_id |= ((uint32_t)data[i+2] << 16);
        packet->m_frame_id |= ((uint32_t)data[i+3] << 8);
        packet->m_frame_id |= ((uint32_t)data[i+4] << 0);

        memcpy(packet->m_frame_data, &data[i+5], FRAME_DATA_LENGTH);

        packet = &packets[number++];

        /*if(packet->m_frame_id != 0x80) {
            printf("CanBus Packet Info, data length = %d\n", packet->m_info & FRAME_INFO_LENGTH_MASK);
            printf("CanBus Packet Frame ID: 0x%02X\n", packet->m_frame_id);
        }*/
    }
    return number;
}


int32_t sm_dk_canopen_if_process(sm_canopen_if_t* _self){
    int rc = -1;
    if(_self->m_server){
        if(!_self->m_session){
            rc = tcp_server_listen(_self->m_server);
            if(rc < 0){
                printf("TCP Server listen ERROR\n");
                sleep(5);
                return rc;
            }

            tcp_session_t* session = tcp_server_accepted(_self->m_server);
            printf("New Session is accepted");
            if(session){
                _self->m_session = session;
                return 0;
            }
        }else{
            uint8_t buf[128];
            int len = tcp_session_recv(_self->m_session, buf, 128, 10);
            if(len > 0){
                sm_canopen_interface_packet_t packet[MAX_PACKET];
                int packet_number = sm_canopen_interface_decode_packet(buf, len, packet);
                if(packet_number <= 0){
                    return 0;
                }
                if(_self->m_callback){
//                    printf("Recv Canbus message\n");
                    _self->m_callback(packet->m_frame_id, packet->m_frame_data);
                }
            }else if(len < 0){
                tcp_session_destroy(_self->m_session);
                _self->m_session = NULL;
            }

        }
    }

    return rc;
}