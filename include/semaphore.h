#pragma once

#include "monitor.h"
#include <limits>

class Semaphore
{
    public:
        using CountType = unsigned long;

    private:
        CountType counter;
        CountType maxCount;
        Mutex mutex;
        Monitor notEmpty;

    public:
        Semaphore(CountType initValue = 0, CountType _maxValue = std::numeric_limits<CountType>::max());

        void give();
        void take();
        bool take(long timeout_ms);
};