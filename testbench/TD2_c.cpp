#include <iostream>
#include "../include/looper.h"
#include "../include/chrono.h"
#include "../include/timespec_utils.h"


int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "You need to specify the number of loops" << std::endl;
        return 1;
    }
    
    double nLoops = std::stod(argv[1]);
    Looper looper;
    Chrono chrono;
    looper.runLoop(nLoops);
    double duration_ms = timespec_to_ms(chrono.stop());

    std::cout << "Duration for " << nLoops << " loops: " << duration_ms << " ms" << std::endl;

    return 0;
}