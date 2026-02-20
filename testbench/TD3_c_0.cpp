#include <iostream>

void* incrementer(void* v_stop)
{
    volatile bool* p_stop = (volatile bool*) v_stop;
    double counter = 0.0;
    while (not *p_stop)
    {
        counter += 1.0;     
    }
    std::cout << "Counter value = " << counter << std::endl;
    return v_stop;
}

int main()
{
    volatile bool stop = false;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    sched_param schedParam;
    schedParam.sched_priority = 9; // Between 1 and 99
    // We can also use sched_get_priority_max(SCHED_RR) for Round Robin, SCHED_FIFO and SCHED_OTHER for default scheduling policy

    pthread_attr_setschedparam(&attr, &schedParam);
    pthread_t incrementThread;
    pthread_create(&incrementThread, &attr, incrementer, (void*) &stop);

    for (char cmd='r'; cmd!='s'; std::cin >> cmd)
    {
        std::cout << "Type 's' to stop: " << std::flush;
    }
    stop = true;

    pthread_attr_destroy(&attr);
    pthread_join(incrementThread, NULL);

    return 0;
}

