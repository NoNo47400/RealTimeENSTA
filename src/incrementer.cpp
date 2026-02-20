#include "../include/incrementer.h"

void Incrementer::run()
{
    for(int i = 0; i < nLoops; ++i)
    {
        r_counter.increment();
    }
}

Incrementer::Incrementer(int id, Counter& counter, double _nLoops_) : Thread(id), r_counter(counter), nLoops(_nLoops_) {}

    