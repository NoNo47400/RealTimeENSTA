#include "../include/looper.h"

double Looper::runLoop(double nLoops)
{
    doStop = false;
    for (iLoop=0; iLoop<nLoops; iLoop++)
    {
        if(doStop==true)
        {
            return iLoop;
        }
    }
    return iLoop;
}

double Looper::getSample() const
{
    return iLoop;
}

double Looper::stopLoop()
{
    doStop = true;
    return iLoop;
}
