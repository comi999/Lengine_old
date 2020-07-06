#include "Timer.h"


    Timer::Timer() : beg_(clock_::now()) {}
    void Timer::reset()
    {
        beg_ = clock_::now(); 
    }
    double Timer::elapsed() const 
    {
        return std::chrono::duration_cast<second_>
            (clock_::now() - beg_).count();
    }
