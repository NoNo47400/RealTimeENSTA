#include "../include/cpu_loop.h"

CpuLoop::CpuLoop(Calibrator& calibrator)
{
    this->calibrator = &calibrator;
}

double CpuLoop::runTime(double duration_ms)
{
    double estimatedLoops = calibrator->nLoops(duration_ms);
    chrono.restart();
    runLoop(estimatedLoops);
    double measuredDuration_ms = timespec_to_ms(chrono.stop());

    // Calcul de l'erreur
    return ((measuredDuration_ms - duration_ms) / duration_ms) * 100.0;
}