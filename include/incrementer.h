#pragma once

#include "thread.h"
#include "counter.h"

class Incrementer : public Thread
{
    private:
        Counter& r_counter;
        const double nLoops;

    protected:
        void run() override;

    public:
        Incrementer(int id, Counter& counter, double _nLoops_);

    
};