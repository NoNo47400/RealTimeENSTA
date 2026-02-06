#pragma once
#include <cfloat>

class Looper 
{
    private:
        bool doStop;
        double iLoop;

    public:
        double runLoop(double nLoops = DBL_MAX);

        double getSample();

        double stopLoop();
};