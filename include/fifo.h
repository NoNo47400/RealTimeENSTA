#pragma once

#include "monitor.h"
#include <limits>
#include <deque>

template<typename T>
class ActiveFifo : protected std::deque<T>
{
    public:
        using CountType = unsigned long;

    private:
        CountType counter;
        CountType maxCount;
        Mutex mutex;
        Monitor notEmpty;
        Monitor notFull;
        std::deque<T> fifo;

    public:
        ActiveFifo(CountType initValue = 0, CountType _maxValue = std::numeric_limits<CountType>::max());

        void push(const T& item);
        T pop();
        void pop(T& item, unsigned int size_data);
};