#include "../include/semaphore.h"

Semaphore::Semaphore(CountType initValue, CountType _maxValue)
    : counter(initValue), maxCount(_maxValue), mutex(), notEmpty(mutex)
{
    if (counter > maxCount) counter = maxCount;
}

void Semaphore::give()
{
    Monitor::Lock lock(notEmpty);
    if (counter < maxCount)
    {
        ++counter;
        notEmpty.notify();
    }
}

void Semaphore::take()
{
    Monitor::Lock lock(notEmpty);
    while (counter == 0)
    {
        lock.wait();
    }
    --counter;
}

bool Semaphore::take(long timeout_ms)
{
    Monitor::Lock lock(notEmpty);
    while (counter == 0)
    {
        try {
            lock.wait(timeout_ms);
        } catch (const Monitor::TimeoutException&) {
            return false;
        }
    }
    --counter;
    return true;
}