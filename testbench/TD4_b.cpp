#include <iostream>
#include <vector>
#include <cstdlib>
#include "../include/semaphore.h"
#include "../include/thread.h"

class TokenProducer : public Thread
{
public:
    TokenProducer(int id, Semaphore& s, unsigned long tokens)
        : Thread(id), produced(0), sem(s), toProduce(tokens) {}

    void run() override
    {
        for (unsigned long i = 0; i < toProduce; ++i)
        {
            sem.give();
            ++produced;
        }
    }
    unsigned long getProduced() const { return produced; }

private:
    unsigned long produced;
    Semaphore& sem;
    unsigned long toProduce;
};

class TokenConsumer : public Thread
{
public:
    TokenConsumer(int id, Semaphore& s)
        : Thread(id), consumed(0), sem(s) {}

    void run() override
    {
        while (true)
        {
            // Detection of completion: if take() fails after 500ms, we assume producers are done
            if (!sem.take(500)) {
                break; 
            }
            ++consumed;
        }
    }
    unsigned long getConsumed() const { return consumed; }

private:
    unsigned long consumed;
    Semaphore& sem;
};

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <num_consumers> <num_producers> <tokens_per_producer>" << std::endl;
        return 1;
    }

    int numConsumers = std::atoi(argv[1]);
    int numProducers = std::atoi(argv[2]);
    unsigned long tokensPerProducer = std::stoul(argv[3]);

    Semaphore sem(0, std::numeric_limits<Semaphore::CountType>::max());

    std::vector<TokenProducer*> producers;
    std::vector<TokenConsumer*> consumers;

    for (int i = 0; i < numProducers; ++i)
        producers.push_back(new TokenProducer(i+1, sem, tokensPerProducer));

    for (int i = 0; i < numConsumers; ++i)
        consumers.push_back(new TokenConsumer(i+1, sem));

    for (auto p : producers) p->start();
    for (auto c : consumers) c->start();

    unsigned long totalProduced = 0;
    for (auto p : producers)
    {
        p->join();
        totalProduced += p->getProduced();
        std::cout << "Producer produced : " << p->getProduced() << " jetons." << std::endl;
    }

    unsigned long totalConsumed = 0;
    for (auto c : consumers)
    {
        c->join();
        totalConsumed += c->getConsumed();
        std::cout << "Consumer consumed : " << c->getConsumed() << " jetons." << std::endl;
    }

    std::cout << "\nTotal produced: " << totalProduced << std::endl;
    std::cout << "Total consumed: " << totalConsumed << std::endl;

    if (totalProduced == totalConsumed) {
        std::cout << "No tokens lost" << std::endl;
    } else {
        std::cout << "Synchronization error: missing or surplus tokens." << std::endl;
    }

    for (auto p : producers) delete p;
    for (auto c : consumers) delete c;

    return 0;
}