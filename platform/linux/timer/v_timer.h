//
// Created by vuonglk on 29/11/2024.
//

#ifndef LIBS_STORAGE_V_TIMER_H
#define LIBS_STORAGE_V_TIMER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
typedef void(*v_timer_callback)(void* arg);

typedef void v_timer_t;

v_timer_t* v_timer_create(uint32_t _interval_ms, v_timer_callback _cb, void* _arg) __wur;

int32_t v_timer_single_shot(uint32_t _interval_ms, v_timer_callback _cb, void* _arg);

int32_t v_timer_delete(v_timer_t* _timer);

#ifdef __cplusplus
}
#endif
#endif //LIBS_STORAGE_V_TIMER_H
