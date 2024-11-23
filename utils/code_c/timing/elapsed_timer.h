//
// Created by vuonglk on 23/03/2023.
//

#ifndef ELAPSED_TIMER_H
#define ELAPSED_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern int64_t get_tick_count();

typedef struct elapsed_timer{
    int64_t m_duration;
    int64_t m_start_time;
}elapsed_timer_t;

static inline void elapsed_timer_reset(elapsed_timer_t* _self){
    _self->m_start_time = get_tick_count();
}

static inline void elapsed_timer_resetz(elapsed_timer_t* _self, int64_t _duration){
    _self->m_duration = _duration;
    _self->m_start_time = get_tick_count();
}

static inline void elapsed_timer_pass(elapsed_timer_t* _self){
    _self->m_duration = 0;
}

static inline int64_t elapsed_timer_get_remain(elapsed_timer_t* _self){
    int64_t passed_time = get_tick_count() - _self->m_start_time;
    if(passed_time >= _self->m_duration){
        return 0;
    }else{
        return _self->m_duration - passed_time;
    }
}

#ifdef __cplusplus
};
#endif

#endif //ELAPSED_TIMER_H
