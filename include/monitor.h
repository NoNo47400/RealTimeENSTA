#pragma once

#include "mutex.h"
#include "timespec_utils.h"
#include <pthread.h>

class Monitor
{
    private:
        pthread_cond_t posixCondId;
        Mutex& mutex;

    public:
        class Lock;
        class TimeoutException;

        Monitor(Mutex& _mutex);
        ~Monitor();

        void notify();
        void notifyAll();
};

class Monitor::Lock : public Mutex::Lock
{
    private:
        Monitor& monitor;

    public:
        Lock(Monitor& _monitor);
        Lock(Monitor& _monitor, long timeout_ms);

        void wait();
        void wait(long timeout_ms);
};

class Monitor::TimeoutException
{
    public:
        const long timeout_ms;
        TimeoutException(long _timeout_ms);
};