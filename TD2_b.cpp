#include "timer.h"
#include <iostream>

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