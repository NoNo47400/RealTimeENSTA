#include "cpu_loop.h"
#include "calibrator.h"
#include <iostream>

int main()
{
    Calibrator calibrator(200.0, 100); // On prend une dizaine d'échantillons tous les 200ms pour la calibration
    std::cout << "Calibration finie" << std::endl;

    CpuLoop cpuLoop(calibrator);

    double error = cpuLoop.runTime(1000.0); // On veut faire tourner le CPU pendant 1s
    std::cout << "Relative error for 1s: " << error << "%" << std::endl;
    return 0;
}