#include "looper.h"

double Looper::runLoop(double nLoops)
{
    iLoop = 0;
    doStop = false;
    for (int i=0; i<nLoops; i++)
    {
        if(doStop==true)
        {
            return iLoop;
        }
    }
    return iLoop;
}

double Looper::getSample()
{
    return iLoop;
}

double Looper::stopLoop()
{
    doStop = true;
    return iLoop;
}
