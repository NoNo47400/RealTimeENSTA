#include <iostream>
#include <vector>
#include <memory>

#include "../include/incrementer.h"
#include "../include/counter.h"
#include "../include/thread.h"
#include "../include/chrono.h"

int main(int argc, char* argv[])
{
    if ((argc < 3) || (std::string(argv[1]) != "--nLoops")) 
    {
        std::cerr << "You need to specify the number of loops using the parameter \"--nLoops\"" << std::endl;
        return 1;
    }

    double nLoops = std::stod(argv[2]);
    std::cout << "Number of loops: " << nLoops << std::endl;

    if ((argc < 5) || (std::string(argv[3]) != "--nTasks")) 
    {
        std::cerr << "You need to specify the number of tasks using the parameter \"--nTasks\"" << std::endl;
        return 1;
    }

    double nTasks = std::stod(argv[4]);
    std::cout << "Number of tasks: " << nTasks << std::endl;

    bool protect = false;
    if ((argc > 5) && (std::string(argv[5]) == "--protect")) 
    {
        protect = true;
    }

    std::cout << "Protect: " << (protect ? "true" : "false") << std::endl;

    int policy = SCHED_OTHER;
    if ((argc > 6) && (std::string(argv[6]) == "--policy")) 
    {
        std::string p = argv[7];
        if (p == "SCHED_RR")
            policy = SCHED_RR;
        else if (p == "SCHED_FIFO")
            policy = SCHED_FIFO;
        else
            policy = SCHED_OTHER;
    }

    std::cout << "Policy: " << (policy == SCHED_RR ? "SCHED_RR" : (policy == SCHED_FIFO ? "SCHED_FIFO" : "SCHED_OTHER")) << std::endl;

    Counter counter(protect);
    std::vector<std::unique_ptr<Incrementer>> incrementers;

    int max_prio = Thread::getMaxPrio(policy);
    Thread::setMainSched(policy);
    Chrono chrono;

    for (int i = 0; i < nTasks; ++i) 
    {
        incrementers.push_back(std::unique_ptr<Incrementer>(new Incrementer(i, counter, nLoops)));
    }
    
    for (int i = 0; i < nTasks; ++i) 
    {
        incrementers[i]->start(max_prio); 
    }

    for (int i = 0; i < nTasks; ++i) 
    {
        incrementers[i]->join();
    }
    double execution_time = chrono.lap_ms();
    std::cout << "Final counter value: " << counter.getValue() << std::endl;
    std::cout << "Expected counter value: " << nTasks * nLoops << std::endl;
    std::cout << "Execution time: " << execution_time << " ms" << std::endl;

    // With a low number of loops and tasks, without protection is faster and is equivalent in the result, but with a high number of loops and tasks, without protection is faster but the result is not equivalent (it is lower than expected because of the race conditions), but if it is protected, the result is equivalent but it is slower because of the overhead of the mutex 
    // To use RR or FIFO, we need to launch the program in root
    return 0;
}