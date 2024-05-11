//
// Created by vuonglk on 31/05/2023.
//

#ifndef BSS_BASE_TIMER_H
#define BSS_BASE_TIMER_H

#include "TimeUtils.h"

class WaitTimer{
        private:
        int64_t m_duration;
        int64_t m_startTime;

        public:
        explicit WaitTimer(int64_t _duration){
            m_duration = _duration;
            m_startTime = getTickCount();
        }

        WaitTimer(const WaitTimer& _copy) = default;

        ~WaitTimer()= default;

        void reset(){
            m_startTime = getTickCount();
        }

        void reset(int64_t _duration){
            m_duration = _duration;
            m_startTime = getTickCount();
        }

        int64_t getRemainTime() const{
            int64_t elapsed;
            elapsed = getTickCount() - m_startTime;
            if (elapsed >= m_duration)
                return 0;
            else
                return m_duration - elapsed;
        }
};

#endif //BSS_BASE_TIMER_H
