#include <iostream>
#include <iomanip>
#include <memory>

#include "../include/thread.h"
#include "../include/mutex.h"
#include "../include/calibrator.h"
#include "../include/cpu_loop.h"
#include "../include/timespec_utils.h"

// We will use this as the reference time execution based on the start of Task C
static timespec start_time_C;

class Thread_A : public Thread
{
    private:
        Mutex& resource;
        CpuLoop& cpu;
        Calibrator& calibrator;
        timespec end_time;

    protected:
        void run() override
        {
            // Is low priority but do a long task with resource locking
            {
                Mutex::Lock lock(resource);
                // Run a loops of 300 ms while locking the resource
                double loops = calibrator.nLoops(300.0);
                cpu.runLoop(loops);
            }

            cpu.runLoop(calibrator.nLoops(50.0));

            end_time = timespec_now();
        }

    public:
        Thread_A(int id, Mutex& r, CpuLoop& c, Calibrator& calib) : Thread(id), resource(r), cpu(c), calibrator(calib) {}

        timespec get_end() const 
        { 
            return end_time; 
        }
};

class Thread_B : public Thread
{
    private:
        CpuLoop& cpu;
        Calibrator& calibrator;
        timespec end_time;

    protected:
        void run() override
        {
            // Medium priority thread with no resource lock
            cpu.runLoop(calibrator.nLoops(400.0));
            end_time = timespec_now();
        }

    public:
        Thread_B(int id, CpuLoop& c, Calibrator& calib) : Thread(id), cpu(c), calibrator(calib) {}

        timespec get_end() const 
        { 
            return end_time; 
        }
};

class Thread_C : public Thread
{
    private:
        Mutex& resource;
        CpuLoop& cpu;
        Calibrator& calibrator;
        timespec end_time;

    protected:
        void run() override
        {
            start_time_C = timespec_now();

            // High priority thread that tries to acquire resource and do short work
            {
                Mutex::Lock lock(resource);
                // Short critical section: 50 ms
                cpu.runLoop(calibrator.nLoops(50.0));
            }

            end_time = timespec_now();
        }

    public:
        Thread_C(int id, Mutex& r, CpuLoop& c, Calibrator& calib) : Thread(id), resource(r), cpu(c), calibrator(calib) {}

        timespec get_end() const 
        { 
            return end_time; 
        }
};

int main()
{
    std::cout << std::fixed << std::setprecision(1);

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

        int prio_high = max_prio;
        int prio_mid = (max_prio + min_prio) / 2;
        int prio_low = min_prio;

        Thread_A task_A(1, resource, cpuA, calibrator);
        Thread_C task_C(2, resource, cpuC, calibrator);
        Thread_B task_B(3, cpuB, calibrator); // For this scenario, we need a medium priority task to potentially preempt the low-priority one if priorities are honored

        // Start low-priority thread first
        task_A.start(prio_low);
        // Small delay to let A enter and lock the resource
        timespec_wait(timespec_from_ms(10.0));

        // Start high-priority C
        task_C.start(prio_high);
        // Small delay so C attempts to lock and potentially blocks
        timespec_wait(timespec_from_ms(10.0));

        // Start medium-priority B (should preempt A if priorities are honored)
        task_B.start(prio_mid);

        // Wait for all to finish
        task_A.join();
        task_B.join();
        task_C.join();

        // Compute and print durations relative to start of C
        double endA = timespec_to_ms(task_A.get_end() - start_time_C);
        double endB = timespec_to_ms(task_B.get_end() - start_time_C);
        double endC = timespec_to_ms(task_C.get_end() - start_time_C);

        std::cout << "Task C end time  (ms from C start): " << endC << " ms" << std::endl;
        std::cout << "Task A end time  (ms from C start): " << endA << " ms" << std::endl;
        std::cout << "Task B end time  (ms from C start): " << endB << " ms" << std::endl;
    }

    return 0;
}
