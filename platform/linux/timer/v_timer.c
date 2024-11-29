//
// Created by vuonglk on 29/11/2024.

#include "v_timer.h"
#include "time.h"
#include <stdlib.h>
#include <signal.h>

#include "sm_logger.h"

#define TAG "unix_timer"

typedef enum{
    TIMER_REMAIN,
    TIMER_SINGLE_SHOT,
    TIMER_LIFE_TYPE_NUMBER
}TIMER_LIFE_TYPE;

typedef struct{
    timer_t m_timer;
    void* m_arg;
    v_timer_callback m_cb;
    TIMER_LIFE_TYPE m_type;
}v_timer_impl_t;

static void timer_handler(union sigval _sv) {
    v_timer_impl_t* this = _sv.sival_ptr;
    if(!this){
        return;
    }

    if(this->m_cb){
        this->m_cb(this->m_arg);
    }

    if(this->m_type == TIMER_SINGLE_SHOT){
        LOG_INF(TAG, "Remove Timer single shot");
        timer_delete(this->m_timer);
        free(this);
    }
}

v_timer_t* v_timer_create(uint32_t _interval_ms, v_timer_callback _cb, void* _arg){
    v_timer_impl_t* this = malloc(sizeof(v_timer_impl_t));
    if(!this){
        return NULL;
    }

    struct sigevent setting;
    struct itimerspec spec;

    setting.sigev_notify = SIGEV_THREAD;
    setting.sigev_notify_function = timer_handler;
    setting.sigev_notify_attributes = NULL;
    setting.sigev_value.sival_int = 0;
    setting.sigev_value.sival_ptr = this;

    if (timer_create(CLOCK_MONOTONIC, &setting, &this->m_timer) == -1) {
        free(this);
        perror("timer_create");
        return NULL;
    }

    div_t sec = div(_interval_ms, 1000);

    spec.it_value.tv_sec = sec.quot;
    spec.it_value.tv_nsec = sec.rem*1000*1000;
    spec.it_interval.tv_sec = sec.quot;
    spec.it_interval.tv_nsec = sec.rem*1000*1000;

    if (timer_settime(this->m_timer, 0, &spec, NULL) == -1) {
        free(this);
        perror("timer_settime");
        return NULL;
    }
    this->m_cb = _cb;
    this->m_arg = _arg;
    LOG_INF(TAG, "Start timer permanent duration %d ms", _interval_ms);
    this->m_type = TIMER_REMAIN;
    return this;
}

int32_t v_timer_single_shot(uint32_t _interval_ms, v_timer_callback _cb, void* _arg){
    v_timer_impl_t* this = malloc(sizeof(v_timer_impl_t));
    if(!this){
        return -1;
    }

    struct sigevent setting;
    struct itimerspec spec;

    setting.sigev_notify = SIGEV_THREAD;
    setting.sigev_notify_function = timer_handler;
    setting.sigev_notify_attributes = NULL;
    setting.sigev_value.sival_int = 0;
    setting.sigev_value.sival_ptr = this;

    if (timer_create(CLOCK_MONOTONIC, &setting, &this->m_timer) == -1) {
        free(this);
        perror("timer_create");
        return -1;
    }

    div_t sec = div(_interval_ms, 1000);

    spec.it_value.tv_sec = sec.quot;  // Timer 1 sẽ hết thời gian sau 2 giây
    spec.it_value.tv_nsec = sec.rem*1000*1000;
    spec.it_interval.tv_sec = 0;  // Timer chỉ chạy một lần
    spec.it_interval.tv_nsec = 0;

    if (timer_settime(this->m_timer, 0, &spec, NULL) == -1) {
        free(this);
        perror("timer_settime");
        return -1;
    }

    LOG_INF(TAG, "Start timer single shot success, callback after %d ms", _interval_ms);
    this->m_type = TIMER_SINGLE_SHOT;
    return 0;
}

int32_t v_timer_delete(v_timer_t* _timer){
    v_timer_impl_t* this = _timer;
    if(!this){
        return -1;
    }
    LOG_INF(TAG, "Stop timer");
    timer_delete(this->m_timer);
    free(this);
    return 0;
}

