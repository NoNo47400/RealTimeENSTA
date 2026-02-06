#pragma once
#include <signal.h>
#include <time.h>
#include "timespec_utils.h"

class Timer
{
    private:
        timer_t tid;

        static void call_callback(int, siginfo_t* si, void*);
    
    protected:
        virtual void callback() = 0;

    public:   
        Timer();

        ~Timer();

        void start(timespec duration, bool isPeriodic);

        void start_ms(double duration_ms, bool isPeriodic);

        void stop();

        
};