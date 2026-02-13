#include <iostream>

void* incrementer(void* v_stop)
{
    volatile bool* p_stop = (volatile bool*) v_stop;
    double counter = 0.0;
    while (not *p_stop)
    {
        counter += 1.0; // Counter adding a nano second each time for the first exercise but on the third, we have 3 tasks so it is way less
    }
    std::cout << "Counter value = " << counter << std::endl;
    return v_stop;
}

int main()
{
    volatile bool stop = false;
    pthread_t incrementThread;
    pthread_create(&incrementThread, nullptr, incrementer, (void*) &stop);
    for (char cmd='r'; cmd!='s'; std::cin >> cmd)
    {
        std::cout << "Type 's' to stop: " << std::flush;
    }
    stop = true;
    pthread_join(incrementThread, nullptr);
}


