#include <time.h>
#include <iostream>

double timespec_to_ms(const timespec& time_ts)
{
    return time_ts.tv_sec * 1.e3 + time_ts.tv_nsec / 1.e6;
}

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

timespec timespec_now()
{
    timespec time_ts;
    clock_gettime(CLOCK_REALTIME, &time_ts);
    return time_ts;
}


timespec timespec_negate(const timespec& time_ts)
{
    timespec result;
    result.tv_sec = -time_ts.tv_sec;
    result.tv_nsec = -time_ts.tv_nsec;

    // Normalize if nanoseconds is negative
    if (result.tv_nsec < 0) {
        result.tv_sec -= 1;
        result.tv_nsec += 1.e9;
    }
    return result;
}

timespec timespec_add(const timespec& time1_ts, const timespec& time2_ts)
{
    timespec result;
    result.tv_sec = time1_ts.tv_sec + time2_ts.tv_sec;
    result.tv_nsec = time1_ts.tv_nsec + time2_ts.tv_nsec;
    if (result.tv_nsec >= 1.e9) {
        result.tv_sec += 1;
        result.tv_nsec -= 1.e9;
    }
    return result;
}

timespec timespec_subtract(const timespec& time1_ts, const timespec& time2_ts)
{
    return timespec_add(time1_ts, timespec_negate(time2_ts));
}

void timespec_wait(const timespec& delay_ts)
{
    nanosleep(&delay_ts, nullptr);
}

timespec operator+ (const timespec& time1_ts, const timespec& time2_ts)
{
    return timespec_add(time1_ts, time2_ts);
}

timespec operator- (const timespec& time1_ts, const timespec& time2_ts)
{
    return timespec_subtract(time1_ts, time2_ts);
}

timespec& operator+= (timespec& time_ts, const timespec& delay_ts)
{
    time_ts = time_ts + delay_ts;
    return time_ts;
}

timespec& operator-= (timespec& time_ts, const timespec& delay_ts)
{
    time_ts = time_ts - delay_ts;
    return time_ts;
}

bool operator== (const timespec& time1_ts, const timespec& time2_ts)
{
    return (time1_ts.tv_sec == time2_ts.tv_sec) && (time1_ts.tv_nsec == time2_ts.tv_nsec);
}

bool operator!= (const timespec& time1_ts, const timespec& time2_ts)
{
    return !(time1_ts == time2_ts);
}

bool operator< (const timespec& time1_ts, const timespec& time2_ts)
{
    if (time1_ts.tv_sec < time2_ts.tv_sec) {
        return true;
    } else if (time1_ts.tv_sec == time2_ts.tv_sec) {
        return (time1_ts.tv_nsec < time2_ts.tv_nsec);
    } else {
        return false;
    }
}

bool operator> (const timespec& time1_ts, const timespec& time2_ts)
{
    if (time1_ts.tv_sec > time2_ts.tv_sec) {
        return true;
    } else if (time1_ts.tv_sec == time2_ts.tv_sec) {
        return (time1_ts.tv_nsec > time2_ts.tv_nsec);
    } else {
        return false;
    }
}

timespec  operator- (const timespec& time_ts)
{
    return timespec_negate(time_ts);
}

class Chrono
{
private:
    timespec m_duration;

public:
    timespec m_startTime;
    timespec m_stopTime;
    Chrono() 
    {
        restart();
    }

    void restart() 
    {
        m_startTime = timespec_now();
        m_stopTime = m_startTime;
    }

    timespec stop() 
    {
        m_stopTime = timespec_now();
        m_duration = m_stopTime - m_startTime;
        return m_duration;
    }

    bool isActive() const 
    {
        return m_startTime == m_stopTime;
    }

    timespec lap()
    {
        if (isActive()) 
        {
            timespec now = timespec_now();
            std::cout << "Now time: " << timespec_to_ms(now) << " ms" << std::endl;
            std::cout << "Start time: " << timespec_to_ms(m_startTime) << " ms" << std::endl;
            timespec lapDuration = now - m_startTime;
            return lapDuration;
        } 
        else 
        {
            return m_duration;
        }
    }

    double lap_ms() 
    {
        return timespec_to_ms(lap());
    }
};

int main()
{
    // Test addition
    double test1_1 = 1500; //ms
    double test1_2 = 2000; //ms
    timespec result_1 = timespec_from_ms(test1_1) + timespec_from_ms(test1_2);
    std::cout << "Result 1: " << timespec_to_ms(result_1) << " ms" << std::endl;

    // Test negative
    double test2_1 = 1500; //ms
    timespec result_2 = -timespec_from_ms(test2_1);
    std::cout << "Result 2_1 timespec: " << result_2.tv_sec << " s " << result_2.tv_nsec << " ns" << std::endl;
    std::cout << "Result 2_1: " << timespec_to_ms(result_2) << " ms" << std::endl;
    double test2_2 = 5000; //ms
    timespec result_2_2 = -timespec_from_ms(test2_2);
    std::cout << "Result 2_2 timespec: " << result_2_2.tv_sec << " s " << result_2_2.tv_nsec << " ns" << std::endl;
    std::cout << "Result 2_2: " << timespec_to_ms(result_2_2) << " ms" << std::endl;
    double test2_3 = -2000; //ms
    timespec result_2_3 = -timespec_from_ms(test2_3);
    std::cout << "Result 2_3 timespec: " << result_2_3.tv_sec << " s " << result_2_3.tv_nsec << " ns" << std::endl;
    std::cout << "Result 2_3: " << timespec_to_ms(result_2_3) << " ms" << std::endl;
    double test2_4 = -2500; //ms
    timespec result_2_4 = -timespec_from_ms(test2_4);
    std::cout << "Result 2_4 timespec: " << result_2_4.tv_sec << " s " << result_2_4.tv_nsec << " ns" << std::endl;
    std::cout << "Result 2_4: " << timespec_to_ms(result_2_4) << " ms" << std::endl;

    // Test addition nombre negatifs
    double test3_1 = -1500; //ms
    double test3_2 = -2000; //ms
    timespec result_3_1 = timespec_from_ms(test3_1) + timespec_from_ms(test3_2);
    std::cout << "Result 3_1: " << timespec_to_ms(result_3_1) << " ms" << std::endl;

    double test3_3 = 1500; //ms
    double test3_4 = -2000; //ms
    timespec result_3_2 = timespec_from_ms(test3_3) + timespec_from_ms(test3_4);
    std::cout << "Result 3_2: " << timespec_to_ms(result_3_2) << " ms" << std::endl;

    // Test soustraction -- PAS BON
    double test4_1 = 5000; //ms
    double test4_2 = 2000; //ms
    timespec result_4_1 = timespec_from_ms(test4_1) - timespec_from_ms(test4_2);
    std::cout << "Result 4_1: " << timespec_to_ms(result_4_1) << " ms" << std::endl;
    timespec result_4_2 = timespec_from_ms(test4_2) - timespec_from_ms(test4_1);
    std::cout << "Result 4_2: " << timespec_to_ms(result_4_2) << " ms" << std::endl;

    return 0;
}