#include "cpu_loop.h"
#include "calibrator.h"
#include <iostream>

int main()
{
    Calibrator calibrator(50.0, 100); // denser sampling: 20 samples every 50ms (covers ~1s)
    std::cout << "Calibration finished" << std::endl;

    CpuLoop cpuLoop(calibrator);

    double error = cpuLoop.runTime(1000.0); // We want to run the CPU for 1s
    std::cout << "Relative error for 1s: " << error << "%" << std::endl;
    return 0;
}