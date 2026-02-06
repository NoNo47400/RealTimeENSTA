#include <signal.h>
#include <time.h>
#include <iostream>

class Timer
{
    private:
        timer_t tid;
        timespec timespec_from_ms(double time_ms)
        {
            timespec time_ts;
            time_ts.tv_sec = (time_ms / 1.e3);
            double remaining_ms = time_ms - (time_ts.tv_sec * 1.e3);
            time_ts.tv_nsec = (remaining_ms * 1.e6);

            // Normalize if nanoseconds is negative
            if (time_ts.tv_nsec < 0) {
                time_ts.tv_sec -= 1;
                time_ts.tv_nsec += 1.e9;
            }
            return time_ts;
        }

        static void call_callback(int, siginfo_t* si, void*) 
        {
            Timer* ptr = static_cast<Timer*>(si->si_value.sival_ptr);
            ptr->callback();
        }
    
    protected:
        virtual void callback() = 0;

    public:   
        Timer()
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

        ~Timer()
        {
            timer_delete(tid);
        }

        void start(timespec duration, bool isPeriodic)
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

        void start_ms(double duration_ms, bool isPeriodic)
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

        void stop() // Will not have effect if the timer hasn't been started
        {
            itimerspec its;
            its.it_value.tv_sec = 0; 
            its.it_value.tv_nsec = 0; 
            timer_settime(tid, 0, &its, nullptr);
        }

        
};

class CountDown : public Timer
{
    private:
        void callback() override
        { 
            timer_cpt--;
            std::cout << "Remaining Time : " << timer_cpt << std::endl;
        }

    public:
        volatile int timer_cpt=0;
        void start_ms(int n, double frequency_ms)
        {
            timer_cpt = n;
            Timer::start_ms(frequency_ms, true);
        }

        void start(int n, timespec frequency)
        {
            timer_cpt = n;
            Timer::start(frequency, true);
        }
};

int main()
{
    CountDown count;
    count.start_ms(10, 1000); // Count down from 10 with a frequency of 1Hz (1000ms)
    while (count.timer_cpt > 0) {
        continue;
    }
}