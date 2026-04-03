#include "../include/fifo.h"

template<typename T>
ActiveFifo<T>::ActiveFifo(typename ActiveFifo<T>::CountType initValue,
                          typename ActiveFifo<T>::CountType _maxValue)
    : counter(initValue),
      maxCount(_maxValue),
      mutex(),
      notEmpty(mutex),
      notFull(mutex)
{
    if (counter > maxCount) counter = maxCount;
}

template<typename T>
T ActiveFifo<T>::pop()
{
    Monitor::Lock lock(notEmpty);
    while (counter == 0)
    {
        lock.wait();
    }
    // retrieve front element and remove it from the deque
    T item = fifo.front();
    fifo.pop_front();
    --counter;
    notFull.notify();
    return item;
}

template<typename T>
void ActiveFifo<T>::pop(T& item, unsigned int size_data)
{
    Monitor::Lock lock(notEmpty);
    while (counter == 0)
    {
        lock.wait();
    }
    // retrieve up to size_data elements and remove them from the deque
    for (unsigned int i = 0; i < size_data && counter > 0; ++i) {
        item = fifo.front();
        fifo.pop_front();
        --counter;
    }
    notFull.notify();
}

template<typename T>
void ActiveFifo<T>::push(const T& item)
{
    Monitor::Lock lock(notFull);
    while (counter >= maxCount)
    {
        lock.wait();
    }
    fifo.push_back(item);
    ++counter;
    notEmpty.notify();
}

// explicit instantiation so linker can find ActiveFifo<char*> symbols
template class ActiveFifo<char*>;