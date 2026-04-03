#include <iostream>
#include <vector>
#include <cstdlib>
#include "../include/fifo.h"
#include "../include/thread.h"

class Producer : public Thread
{
public:
    Producer(int id, ActiveFifo<char*>& f, unsigned long tokens)
        : Thread(id), produced(0), fifo(f), toProduce(tokens) {}

    void run() override
    {
        for (unsigned long i = 0; i < toProduce; ++i)
        {
            char* data = new char[50];
            snprintf(data, 50, "Producer %d token %lu", id, i);
            printf("Producer %d produced token %lu\n", id, i);
            fifo.push(data);
            ++produced;
        }
    }
    unsigned long getProduced() const { return produced; }

private:
    unsigned long produced;
    ActiveFifo<char*>& fifo;
    unsigned long toProduce;
};

class Consumer : public Thread
{
public:
    Consumer(int id, ActiveFifo<char*>& f)
        : Thread(id), consumed(0), fifo(f) {}

    void run() override
    {
        while (true)
        {
            // use single-item pop and exit on nullptr sentinel
            char* data = fifo.pop();
            if (data == nullptr) break;
            printf("Consumer %d consumed: %s\n", id, data);
            ++consumed;
            delete[] data;
        }
    }
    unsigned long getConsumed() const { return consumed; }

private:
    unsigned long consumed;
    ActiveFifo<char*>& fifo;
};

int main(int argc, char** argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0] << " <num_consumers> <num_producers> <messages_per_producer> <buffer_size>" << std::endl;
        return 1;
    }

    int numConsumers = std::atoi(argv[1]);
    int numProducers = std::atoi(argv[2]);
    unsigned long messagesPerProducer = std::stoul(argv[3]);
    unsigned long bufferSize = std::stoul(argv[4]);

    ActiveFifo<char*> fifo(0, bufferSize);

    std::vector<Producer*> producers;
    std::vector<Consumer*> consumers;

    for (int i = 0; i < numProducers; ++i)
        producers.push_back(new Producer(i+1, fifo, messagesPerProducer));

    for (int i = 0; i < numConsumers; ++i)
        consumers.push_back(new Consumer(i+1, fifo));

    for (auto p : producers) p->start();
    for (auto c : consumers) c->start();

    unsigned long totalProduced = 0;
    for (auto p : producers)
    {
        p->join();
        totalProduced += p->getProduced();
    }

    // send termination sentinels for each consumer
    for (int i = 0; i < numConsumers; ++i) {
        fifo.push(nullptr);
    }

    unsigned long totalConsumed = 0;
    for (auto c : consumers)
    {
        c->join();
        totalConsumed += c->getConsumed();
    }

    std::cout << "\nTotal produced: " << totalProduced << std::endl;
    std::cout << "Total consumed: " << totalConsumed << std::endl;

    if (totalProduced == totalConsumed) {
        std::cout << "No data lost" << std::endl;
    } else {
        std::cout << "Synchronization error: missing or surplus messages." << std::endl;
    }

    for (auto p : producers) delete p;
    for (auto c : consumers) delete c;

    return 0;
}