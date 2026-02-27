#include <iostream>
#include <iomanip>
#include <memory>
#include <sched.h>

#include "../include/thread.h"
#include "../include/mutex.h"
#include "../include/calibrator.h"
#include "../include/cpu_loop.h"
#include "../include/timespec_utils.h"

class Thread_A : public Thread
{
    private:
        Mutex& resource;
        CpuLoop& cpu;
        Calibrator& calibrator;

        // activation and timing parameters (ms)
        double exec_ms;
        double request_after_ms; // Time to wait before asking for the resource
        double hold_ms; // Time to hold the resource

    protected:
        void run() override
        {
            // Execute until request moment
            if (request_after_ms > 0.0) {
                cpu.runLoop(calibrator.nLoops(request_after_ms));
            }

            // Request resource R
            {
                Mutex::Lock lock(resource);
                // Hold resource for hold_ms
                cpu.runLoop(calibrator.nLoops(hold_ms));
            }

            // Finish the execution
            double remaining_ms = exec_ms - request_after_ms - hold_ms;
            if (remaining_ms > 0.0) {
                cpu.runLoop(calibrator.nLoops(remaining_ms));
            }
        }

    public:
        Thread_A(int id, Mutex& r, CpuLoop& c, Calibrator& calib, double exec_ms_, double request_after_ms_, double hold_ms_) : Thread(id), resource(r), cpu(c), calibrator(calib), exec_ms(exec_ms_), request_after_ms(request_after_ms_), hold_ms(hold_ms_) {}
};

class Thread_B : public Thread
{
    private:
        CpuLoop& cpu;
        Calibrator& calibrator;
        double exec_ms;

    protected:
        void run() override
        {
            cpu.runLoop(calibrator.nLoops(exec_ms));
        }

    public:
    Thread_B(int id, CpuLoop& c, Calibrator& calib, double exec_ms_) : Thread(id), cpu(c), calibrator(calib), exec_ms(exec_ms_) {}
};

class Thread_C : public Thread
{
    private:
        Mutex& resource;
        CpuLoop& cpu;
        Calibrator& calibrator;

        // activation and timing parameters (ms)
        double exec_ms;
        double request_after_ms; // Time to wait before asking for the resource
        double hold_ms; // Time to hold the resource

    protected:
        void run() override
        {
            // Execute until request moment
            if (request_after_ms > 0.0) {
                cpu.runLoop(calibrator.nLoops(request_after_ms));
            }

            // Request resource R
            {
                Mutex::Lock lock(resource);
                // Hold resource for hold_ms
                cpu.runLoop(calibrator.nLoops(hold_ms));
            }

            // Finish the execution
            double remaining_ms = exec_ms - request_after_ms - hold_ms;
            if (remaining_ms > 0.0) {
                cpu.runLoop(calibrator.nLoops(remaining_ms));
            }
        }

    public:
        Thread_C(int id, Mutex& r, CpuLoop& c, Calibrator& calib, double exec_ms_, double request_after_ms_, double hold_ms_) : Thread(id), resource(r), cpu(c), calibrator(calib), exec_ms(exec_ms_), request_after_ms(request_after_ms_), hold_ms(hold_ms_) {}
};

int main()
{
    std::cout << std::fixed << std::setprecision(1);

    // To be sure we only use one core
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0) {
        std::cerr << "Failed to set CPU affinity" << std::endl;
        return 1;
    }

    std::cout << "Running on CPU 0 only" << std::endl;

    // We'll run two passes: without priority inheritance, then with it
    for (int pass = 0; pass < 2; ++pass)
    {
        bool protect = (pass == 1);
        std::cout << "\n=== Pass " << (pass+1) << " : priority inheritance " << (protect ? "ENABLED" : "DISABLED") << " ===\n";

        Mutex resource(protect); // Shared resource

        // 10 ms system tick 
        Calibrator calibrator(10.0, 10);

        CpuLoop cpuA(calibrator);
        CpuLoop cpuB(calibrator);
        CpuLoop cpuC(calibrator);

        int policy = SCHED_RR; // Round Robin is the better choice for real-time priorities 
        Thread::setMainSched(policy);
        int max_prio = Thread::getMaxPrio(policy);
        int min_prio = Thread::getMinPrio(policy);

        int prio_high = max_prio-3;
        int prio_mid = (max_prio + min_prio) / 2;
        int prio_low = min_prio;

        Thread_A task_A(1, resource, cpuA, calibrator, 4000, 1000, 1000); // We multiply the times by 10 to have a tick of 10ms
        Thread_C task_C(2, resource, cpuC, calibrator, 5000, 2000, 2000);
        Thread_B task_B(3, cpuB, calibrator, 1000); // For this scenario, we need a medium priority task to potentially preempt the low-priority one if priorities are honored

        // Start low-priority thread first
        task_C.start(prio_low);
        // Small delay to let A enter and lock the resource
        timespec_wait(timespec_from_ms(3000.0));

        task_A.start(prio_high);
        task_B.start(prio_mid);

        // Wait for all to finish
        task_A.join();
        task_B.join();
        task_C.join();

        // Compute and print durations relative to start of C
        double endA = task_A.duration_ms();
        double endB = task_B.duration_ms();
        double endC = task_C.duration_ms();

        std::cout << "Task C execution time: " << endC << " ms (equivalent to " << (endC / 10.0) << " ticks)" << std::endl;
        std::cout << "Task A execution time: " << endA << " ms (equivalent to " << (endA / 10.0) << " ticks)" << std::endl;
        std::cout << "Task B execution time: " << endB << " ms (equivalent to " << (endB / 10.0) << " ticks)" << std::endl;
    }

    return 0;
}





