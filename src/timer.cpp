#include "../include/timer.h"

void Timer::call_callback(int, siginfo_t* si, void*) 
{
    Timer* ptr = static_cast<Timer*>(si->si_value.sival_ptr);
    ptr->callback();
}

Timer::Timer()
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = call_callback; 
    sigemptyset(&sa.sa_mask);
    sigaction(SIGRTMIN, &sa, nullptr);

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = (void*) this;

    timer_create(CLOCK_REALTIME, &sev, &tid);
}

Timer::~Timer()
{
    timer_delete(tid);
}

void Timer::start(timespec duration, bool isPeriodic)
{
    itimerspec its;
    its.it_value.tv_sec = duration.tv_sec; 
    its.it_value.tv_nsec = duration.tv_nsec; 
    if (isPeriodic)
    {
        its.it_interval.tv_sec = duration.tv_sec;
        its.it_interval.tv_nsec = duration.tv_nsec;
    }   
    timer_settime(tid, 0, &its, nullptr);
}

void Timer::start_ms(double duration_ms, bool isPeriodic)
{
    timespec duration = timespec_from_ms(duration_ms);
    itimerspec its;
    its.it_value.tv_sec = duration.tv_sec; 
    its.it_value.tv_nsec = duration.tv_nsec; 
    if (isPeriodic)
    {
        its.it_interval.tv_sec = duration.tv_sec;
        its.it_interval.tv_nsec = duration.tv_nsec;
    }   
    timer_settime(tid, 0, &its, nullptr);
}

void Timer::stop() // Will not have effect if the timer hasn't been started
{
    itimerspec its;
    its.it_value.tv_sec = 0; 
    its.it_value.tv_nsec = 0; 
    timer_settime(tid, 0, &its, nullptr);
}

