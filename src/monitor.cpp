#include "../include/monitor.h"
#include <cerrno>

Monitor::Monitor(Mutex& _mutex) : mutex(_mutex)
{
    pthread_cond_init(&posixCondId, nullptr);
}

Monitor::~Monitor()
{
    pthread_cond_destroy(&posixCondId);
}

void Monitor::notify()
{
    pthread_cond_signal(&posixCondId);
}

void Monitor::notifyAll()
{
    pthread_cond_broadcast(&posixCondId);
}

Monitor::Lock::Lock(Monitor& _monitor) : Mutex::Lock(_monitor.mutex), monitor(_monitor) {}

Monitor::Lock::Lock(Monitor& _monitor, long timeout_ms) : Mutex::Lock(_monitor.mutex, timeout_ms), monitor(_monitor) {}

void Monitor::Lock::wait()
{
    pthread_cond_wait(&monitor.posixCondId, monitor.mutex.getPosixMutexId());
}

void Monitor::Lock::wait(long timeout_ms)
{
    timespec deadline = timespec_now() + timespec_from_ms(timeout_ms);
    int ret = pthread_cond_timedwait(&monitor.posixCondId, monitor.mutex.getPosixMutexId(), &deadline);
    if (ret == ETIMEDOUT) {
        throw Monitor::TimeoutException(timeout_ms);
    }
}

Monitor::TimeoutException::TimeoutException(long _timeout_ms) : timeout_ms(_timeout_ms) {}