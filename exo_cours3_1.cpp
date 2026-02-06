#include <signal.h>
#include <time.h>
#include <iostream>

struct myData {
    int timer_cpt=0;
};

void myHandler(int sig, siginfo_t* si, void*)
{
    myData* data_ptr = static_cast<myData*>(si->si_value.sival_ptr);
    data_ptr->timer_cpt++;
}



int main()
{
    myData data;

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = myHandler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGRTMIN, &sa, nullptr);

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = (void*) &data;

    timer_t tid;
    timer_create(CLOCK_REALTIME, &sev, &tid);
    itimerspec its;
    its.it_value.tv_sec = 0; 
    its.it_value.tv_nsec = 500000000; // 500 ms = 2Hz
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 500000000; // 500 ms = 2Hz
    int timer_cpt_diff = 0;
    std::cout << "Counter value: " << data.timer_cpt << std::endl;
    timer_settime(tid, 0, &its, nullptr);
    while (data.timer_cpt <= 15) {
        if (data.timer_cpt > timer_cpt_diff) {
            timer_cpt_diff = data.timer_cpt;
            std::cout << "Counter value: " << data.timer_cpt << std::endl;
        }
    }
}