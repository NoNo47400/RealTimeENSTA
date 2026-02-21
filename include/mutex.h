#pragma once

#include <iostream>
#include <pthread.h>

#include "timespec_utils.h"


class Mutex 
{
    private:
        pthread_mutex_t posixMutexId;
        pthread_mutexattr_t posixMutexAttrId;

        void lock();
        bool lock(double timeout_ms);
        void unlock();

    public:
        class Lock;
        class TimeoutException;
        Mutex(bool isInversionSafe = false);
        ~Mutex();
};


class Mutex::Lock
{
    private:
        Mutex& mutex;
    public:
        Lock(Mutex& _mutex);
        Lock(Mutex& _mutex, double timeout_ms);
        ~Lock();
};

class Mutex::TimeoutException 
{
    public:
        const long timeout_ms;
        TimeoutException(long _timeout_ms);
};