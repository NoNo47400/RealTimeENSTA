#include "../include/looper.h"
#include "../include/chrono.h"
#include "../include/calibrator.h"
#include <iostream>

int main()
{
    Looper looper;
    Chrono chrono;
    Calibrator calibrator(200.0, 50); // We take ten samples every 200ms for calibration

    std::cout << "Calibration finished" << std::endl;
    std::cout << "Duration (ms) | Estimated nLoops | Measured Duration (ms) | Error (%)" << std::endl;

    for (double duration_ms = 500.0; duration_ms <= 10000.0; duration_ms += 200.0) {
        double estimatedLoops = calibrator.nLoops(duration_ms); // We estimate the number of loops for this duration

        chrono.restart();
        looper.runLoop(estimatedLoops);
        double measuredDuration_ms = timespec_to_ms(chrono.stop());
        
        // Error calculation: relative error in percentage
        double error = ((measuredDuration_ms - duration_ms) / duration_ms) * 100.0;
        std::cout << duration_ms << " | " << estimatedLoops << " | " << measuredDuration_ms << " | " << error << "%" << std::endl;
    }
    
    return 0;
}