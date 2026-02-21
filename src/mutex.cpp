#include "../include/mutex.h"

Mutex::Mutex(bool isInversionSafe)
{
    pthread_mutexattr_init(&posixMutexAttrId);

    // Possible values PTHREAD_MUTEX_DEFAULT, PTHREAD_MUTEX_ERRORCHECK, PTHREAD_MUTEX_RECURSIVE, PTHREAD_MUTEX_NORMAL
    pthread_mutexattr_settype(&posixMutexAttrId, PTHREAD_MUTEX_DEFAULT);

    if (isInversionSafe)
    {
        // Use recursive mutex type and enable priority inheritance protocol
        pthread_mutexattr_settype(&posixMutexAttrId, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutexattr_setprotocol(&posixMutexAttrId, PTHREAD_PRIO_INHERIT);
    }

    pthread_mutex_init(&posixMutexId, &posixMutexAttrId);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&posixMutexId);
    pthread_mutexattr_destroy(&posixMutexAttrId);
}

void Mutex::lock()
{
    pthread_mutex_lock(&posixMutexId);
}

bool Mutex::lock(double timeout_ms)
{
    struct timespec deadline;
    deadline = timespec_now() + timespec_from_ms(timeout_ms);
    return pthread_mutex_timedlock(&posixMutexId, &deadline) == 0; // If it worked, we return True
}

void Mutex::unlock()
{
    pthread_mutex_unlock(&posixMutexId);
}

Mutex::TimeoutException::TimeoutException(long _timeout_ms) : timeout_ms(_timeout_ms) {}

// We now create a variable as a lock so it is on the stack and will be deleted automaticly (and the destructor will be call) if we quit the critic section
// In the main we will do Mutex::Lock lock(mutex)
// We also need to put it between "{}" to create a critical section
Mutex::Lock::Lock(Mutex& _mutex) : mutex(_mutex)
{
    this->mutex.lock();
}

Mutex::Lock::Lock(Mutex& _mutex, double timeout_ms) : mutex(_mutex) // It is a const value so we can not change it normally in the constructor
{
    if (!this->mutex.lock(timeout_ms)) 
    {
        throw Mutex::TimeoutException(long(timeout_ms));
    }
}

Mutex::Lock::~Lock()
{
    this->mutex.unlock();
}

