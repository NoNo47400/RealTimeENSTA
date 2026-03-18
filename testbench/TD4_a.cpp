#include <iostream>
#include "../include/monitor.h"
#include "../include/thread.h"

class Producer : public Thread
{
    public:
        Producer(int id, Monitor& m, int& slot, bool& full, int count) 
            : Thread(id), monitor(m), buffer(slot), hasItem(full), produceCount(count) {}

    protected:
        void run() override
        {
            for (int i = 0; i < produceCount; ++i)
            {
                Monitor::Lock lock(monitor);
                while (hasItem) { lock.wait(); }
                
                buffer = i;
                hasItem = true;
                std::cout << "Producer produced " << buffer << std::endl;
                monitor.notify();
            }
        }

    private:
        Monitor& monitor;
        int& buffer;
        bool& hasItem;
        int produceCount;
};

class Consumer : public Thread
{
    public:
        Consumer(int id, Monitor& m, int& slot, bool& full, int count) 
            : Thread(id), monitor(m), buffer(slot), hasItem(full), consumeCount(count) {}

    protected:
        void run() override
        {
            for (int i = 0; i < consumeCount; ++i)
            {
                Monitor::Lock lock(monitor);
                while (!hasItem) { lock.wait(); }
                
                int value = buffer;
                hasItem = false;
                std::cout << "Consumer consumed " << value << std::endl;
                monitor.notify();
            }
        }

    private:
        Monitor& monitor;
        int& buffer;
        bool& hasItem;
        int consumeCount;
};

int main()
{
    Mutex mutex;
    Monitor monitor(mutex);

    int slot = -1;
    bool full = false;
    const int N = 10;

    Producer producer(1, monitor, slot, full, N);
    Consumer consumer(2, monitor, slot, full, N);

    producer.start();
    consumer.start();

    producer.join();
    consumer.join();

    std::cout << "Test Monitor finished" << std::endl;
    return 0;
}