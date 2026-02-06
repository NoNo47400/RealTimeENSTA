#pragma once
#include "looper.h"
#include "calibrator.h"
#include "chrono.h"

class CpuLoop : public Looper
{
    private:
        Calibrator* calibrator;
        Chrono chrono;

    public:
        CpuLoop(Calibrator& calibrator);

        double runTime(double duration_ms);
};