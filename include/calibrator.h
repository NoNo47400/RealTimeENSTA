#pragma once
#include "looper.h"
#include "timer.h"
#include <vector>
#include <cmath>

class Calibrator : public Timer
{
    private:
        double a;
        double b;
        std::vector<double> samples;
        Looper looper;
        unsigned int nSamples;
        volatile unsigned int timer_cpt = 0;

        void linearRegression(double samplingPeriod_ms);

    protected:
        void callback() override;

    public:
        Calibrator(double samplingPeriod_ms, unsigned int nSamples_);
        double nLoops(double duration_ms) const;
};