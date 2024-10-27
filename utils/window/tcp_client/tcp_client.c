//
// Created by vnbk on 17/03/2023.
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sm_logger.h"

#define TAG "eth"
#include <Ws2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>

#include "tcp_client.h"

struct tcp_client{
    int fd;
};

static tcp_client_t g_tcp_client_df = {
        .fd = -1
};

tcp_client_t* tcp_client_create(){
    tcp_client_t * obj = (tcp_client_t*)malloc(sizeof(tcp_client_t));
    if(!obj){
        return NULL;
    }
    obj->fd = -1;

    return obj;
}

tcp_client_t* tcp_client_create_default(){
    g_tcp_client_df.fd = -1;

    return &g_tcp_client_df;
}

void tcp_client_destroy(tcp_client_t* self){
    if(self){
        tcp_client_disconnect(self);
        free(self);
    }
}

int tcp_client_connect(tcp_client_t* self, const char* host, int port){
    struct sockaddr_in serv_addr;
    if(self){
        if(tcp_client_is_connected(self)){
            tcp_client_disconnect(self);
        }
        int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(fd < 0){
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        // Convert IPv4 and IPv6 addresses from text to binary
        // form
        if (inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0) {
            LOG_INF(TAG, "\nInvalid address/ Address not supported \n");
            return -1;
        }

        if (connect(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            LOG_INF(TAG, "\nConnection Failed \n");
            return -1;
        }
        self->fd = fd;
        return fd;
    }
    return -1;
}

int tcp_client_reconnect(tcp_client_t* self, const char* host, int port){

    struct sockaddr_in serv_addr;
    if(self){
        if(tcp_client_is_connected(self)){
            tcp_client_disconnect(self);
        }
        int fd = self->fd;
        if(fd < 0){
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        // Convert IPv4 and IPv6 addresses from text to binary
        // form
        if (inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0) {
            LOG_INF(TAG, "Invalid address/ Address not supported \n");
            return -1;
        }

        if (connect(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            LOG_INF(TAG, "Connection Failed \n");
            return -1;
        }

        self->fd = fd;
        return fd;
    }
    return -1;
}

int tcp_client_disconnect(tcp_client_t* self){
    if(self){
        if(self->fd <0)
            return 0;

#ifdef __linux
        close(self->fd);
#endif

#ifdef _WIN32
        shutdown(self->fd, SD_BOTH);
        closesocket(self->fd);
#endif
        self->fd = -1;
    }
    return -1;
}

int tcp_client_is_connected(tcp_client_t* self){
    return (self && self->fd < 0) ? 0 : 1;
}

int tcp_client_send(tcp_client_t* self, unsigned char* data, int len, int timeout){

    if(self && tcp_client_is_connected(self)){
        return (int)send(self->fd, data, len, 0);
    }
    return -1;
}

int tcp_client_recv(tcp_client_t* self, unsigned char* buf, int max_len, int ms){
    if(self){
        // struct timeval timeout = {
        //         .tv_sec = ms/1000,
        //         .tv_usec = (ms%1000)*10000};
        // setsockopt(self->fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
        // LOG_INF(TAG, "recv");
        // return (int)recv(self->fd, buf, max_len, 0);
        fd_set readFd;
        struct timeval timeout = {
                .tv_sec = ms/1000,
                .tv_usec = (ms%1000)*10000};
        int result;
        //
        FD_ZERO(&readFd);
        FD_SET(self->fd, &readFd);
        int max = self->fd + 1;  // =FD_SETSIZE

        result = select(max, &readFd, NULL, NULL, &timeout);
        if(result > 0){
            result = (int)recv(self->fd, buf, max_len, 0);
        }
        return result;
    }
    return -1;
}
int tcp_client_get_fd(tcp_client_t* self){
    return self->fd;
}
