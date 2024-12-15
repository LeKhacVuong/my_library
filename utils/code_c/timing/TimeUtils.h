//
// Created by vuonglk on 31/05/2023.
//

#ifndef BSS_BASE_TIMEUTILS_H
#define BSS_BASE_TIMEUTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

static inline int64_t getTickCount(){ //change base on device
    struct timespec ts = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)(ts.tv_sec*1000 + ts.tv_nsec/1000000);
}

static inline void delay(int32_t _second){
    int64_t timeout = getTickCount() + _second*1000;
    while (timeout > getTickCount());
}

static inline void delayMs(int32_t _ms){
    int64_t timeout = getTickCount() + _ms;
    while (timeout > getTickCount());
}


#ifdef __cplusplus
};
#endif

#endif //BSS_BASE_TIMEUTILS_H
