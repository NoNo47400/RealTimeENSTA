#include <iostream>

#include "../include/chrono.h"
#include "../include/timespec_utils.h"

int main()
{
    // Test addition
    double test1_1 = 1500; //ms
    double test1_2 = 2000; //ms
    timespec result_1 = timespec_from_ms(test1_1) + timespec_from_ms(test1_2);
    std::cout << "Result 1: " << timespec_to_ms(result_1) << " ms" << std::endl;

    // Test negative
    double test2_1 = 1500; //ms
    timespec result_2 = -timespec_from_ms(test2_1);
    std::cout << "Result 2_1 timespec: " << result_2.tv_sec << " s " << result_2.tv_nsec << " ns" << std::endl;
    std::cout << "Result 2_1: " << timespec_to_ms(result_2) << " ms" << std::endl;
    double test2_2 = 5000; //ms
    timespec result_2_2 = -timespec_from_ms(test2_2);
    std::cout << "Result 2_2 timespec: " << result_2_2.tv_sec << " s " << result_2_2.tv_nsec << " ns" << std::endl;
    std::cout << "Result 2_2: " << timespec_to_ms(result_2_2) << " ms" << std::endl;
    double test2_3 = -2000; //ms
    timespec result_2_3 = -timespec_from_ms(test2_3);
    std::cout << "Result 2_3 timespec: " << result_2_3.tv_sec << " s " << result_2_3.tv_nsec << " ns" << std::endl;
    std::cout << "Result 2_3: " << timespec_to_ms(result_2_3) << " ms" << std::endl;
    double test2_4 = -2500; //ms
    timespec result_2_4 = -timespec_from_ms(test2_4);
    std::cout << "Result 2_4 timespec: " << result_2_4.tv_sec << " s " << result_2_4.tv_nsec << " ns" << std::endl;
    std::cout << "Result 2_4: " << timespec_to_ms(result_2_4) << " ms" << std::endl;

    // Test addition nombre negatifs
    double test3_1 = -1500; //ms
    double test3_2 = -2000; //ms
    timespec result_3_1 = timespec_from_ms(test3_1) + timespec_from_ms(test3_2);
    std::cout << "Result 3_1: " << timespec_to_ms(result_3_1) << " ms" << std::endl;

    double test3_3 = 1500; //ms
    double test3_4 = -2000; //ms
    timespec result_3_2 = timespec_from_ms(test3_3) + timespec_from_ms(test3_4);
    std::cout << "Result 3_2: " << timespec_to_ms(result_3_2) << " ms" << std::endl;

    // Test soustraction
    double test4_1 = 5000; //ms
    double test4_2 = 2000; //ms
    timespec result_4_1 = timespec_from_ms(test4_1) - timespec_from_ms(test4_2);
    std::cout << "Result 4_1: " << timespec_to_ms(result_4_1) << " ms" << std::endl;
    timespec result_4_2 = timespec_from_ms(test4_2) - timespec_from_ms(test4_1);
    std::cout << "Result 4_2: " << timespec_to_ms(result_4_2) << " ms" << std::endl;

    double test4_3 = 2300; //ms
    double test4_4 = 4100; //ms
    timespec result_4_3 = timespec_from_ms(test4_3) - timespec_from_ms(test4_4);
    std::cout << "Result 4_3: " << timespec_to_ms(result_4_3) << " ms" << std::endl;
    timespec result_4_4 = timespec_from_ms(test4_4) - timespec_from_ms(test4_3);
    std::cout << "Result 4_4: " << timespec_to_ms(result_4_4) << " ms" << std::endl;

    // Test Bool
    double test5_1 = 2000; //ms
    double test5_2 = 3000;
    std::cout << "Result 5_1: " << (timespec_from_ms(test5_1) != timespec_from_ms(test5_2)) << " inequal" << std::endl;
    std::cout << "Result 5_2: " << (timespec_from_ms(test5_1) == timespec_from_ms(test5_2)) << " equal" << std::endl;
    std::cout << "Result 5_3: " << (timespec_from_ms(test5_1) > timespec_from_ms(test5_2)) << " superior" << std::endl;
    std::cout << "Result 5_4: " << (timespec_from_ms(test5_1) < timespec_from_ms(test5_2)) << " inferior" << std::endl;

    // Test Now
    timespec now = timespec_now();
    std::cout << "Now time: " << timespec_to_ms(now) << " ms" << std::endl;

    return 0;
}