#include <iostream>

#include "../include/mutex.h"

struct Data {
    volatile bool stop;
    volatile double counter;
    Mutex mutex;
    Data() : stop(false), counter(0.0) {}
};

void* incrementer(void* v_data)
{
    Data* p_data = (Data*) v_data;
    while (not p_data->stop)
    {
        try
        {
            Mutex::Lock lock(p_data->mutex, 10.0);
            p_data->counter += 1.0; 
        }
        catch (const Mutex::TimeoutException& e)
        {
            std::cerr << "TimeoutException: Delayed execution exceeded: " << e.timeout_ms << " ms" << std::endl;
        }
    }
    return v_data;
}

int main()
{
    Data data;
    pthread_t incrementThread[3];
    pthread_create(&incrementThread[0], nullptr, incrementer, &data);
    pthread_create(&incrementThread[1], nullptr, incrementer, &data);
    pthread_create(&incrementThread[2], nullptr, incrementer, &data);
    for (char cmd='r'; cmd!='s'; std::cin >> cmd)
    {
        std::cout << "Type 's' to stop: " << std::flush;
    }
    data.stop = true;
    for (int i=0; i<3; ++i)
    {
        pthread_join(incrementThread[i], nullptr);
    }
    std::cout << "Counter value = " << data.counter << std::endl;
}


