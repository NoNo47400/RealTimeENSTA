#include <iostream>

#include "../include/mutex.h"
#include "../include/thread.h"

struct Data {
    volatile bool stop;
    volatile double counter;
    Mutex mutex;
    Data() : stop(false), counter(0.0) {}
};

class Incrementer : public Thread
{
    public:
        Data data;
        Incrementer(int id) : Thread(id) {}

    protected:
        void run() override
        {
            while (not data.stop)
            {
                try
                {
                    Mutex::Lock lock(data.mutex, 10.0);
                    data.counter += 1.0; 
                }
                catch (const Mutex::TimeoutException& e)
                {
                    std::cerr << "TimeoutException: Delayed execution exceeded: " << e.timeout_ms << " ms" << std::endl;
                }
            }
        }
};


int main()
{
    Incrementer incrementer[3] = { Incrementer(1), Incrementer(2), Incrementer(3) };

    int max_prio = Thread::getMaxPrio(SCHED_OTHER);
    int min_prio = Thread::getMinPrio(SCHED_OTHER);
    std::cout << "Max priority for SCHED_OTHER: " << max_prio << std::endl;
    std::cout << "Min priority for SCHED_OTHER: " << min_prio << std::endl;

    max_prio = Thread::getMaxPrio(SCHED_FIFO);
    min_prio = Thread::getMinPrio(SCHED_FIFO);
    std::cout << "Max priority for SCHED_FIFO: " << max_prio << std::endl;
    std::cout << "Min priority for SCHED_FIFO: " << min_prio << std::endl;

    Thread::setMainSched(SCHED_OTHER);
    std::cout << "Actual policy: " << Thread::getMainSched() << std::endl;
    Thread::setMainSched(SCHED_FIFO);
    std::cout << "Actual policy: " << Thread::getMainSched() << std::endl;

    for (int i=0; i<3; ++i)
    {
        incrementer[i].start();
    }

    for (char cmd='r'; cmd!='s'; std::cin >> cmd)
    {
        std::cout << "Type 's' to stop: " << std::flush;
    }

    for (int i=0; i<3; ++i)
    {
        incrementer[i].data.stop = true;
        incrementer[i].join();

        std::cout << "Thread ID: " << incrementer[i].id << ", Counter value = " << incrementer[i].data.counter << std::endl;
    }

    return 0;
}


// Il faut faire try, catch dans le main et call_run car c'est un mecanisme qui remonte