#include "timespec_utils.h"

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