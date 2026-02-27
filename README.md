# CSC_RRO05_TA

## Developer notes
I keep a small git alias to push to two remotes in one command:
```
git config --global alias.pushboth '!git push origin && git push origin_ensta'
```
Use it with:
```
git pushboth
```

## Build
To build everything, run:
```
make
```

All executables produced from the `testbench/` sources are placed in the `bin/` directory.


## SSH connection

To connect to the Raspberry Pi, use one of these two methods:

- If you're connected to the ENSTA VPN:
```
ssh -Y <your-dev-login>@147.250.8.198
```

- If you're outside the ENSTA network:
```
ssh -tX <your-ensta-login>@ssh.ensta.fr <your-rpi2b-dev-login>@147.250.8.198
```

<small>Note: You will first be prompted for your ENSTA password, then for your Raspberry Pi password.</small>

After that, you'll be able to copy an executable to the Raspberry Pi and run it:
```
scp -r <directory> <your-dev-login>@147.250.8.198: .
arm-linux-g++ -Wall -Wextra <files_to_compile> -o td
rsync -avz td root@192.168.50.xx:
ssh root@192.168.50.xx
./td
```

<small>Note: To replace de _xx_ in the IP adress, please, connected at the Raspberry, launch the command `cat .raspberry_number` to get you Raspberry number.

## TD descriptions (what the code does)
This repository contains small exercises that demonstrate timing utilities, CPU-consuming loops, basic POSIX timers, threads and mutex usage.

### TD1
This exercise tests the time utilities in `timespec_utils` and the `Chrono` helper class. `Chrono` uses the functions in `timespec_utils` to capture a start time and compute elapsed time when `stop()` is called. It also provides `lap()` and `lap_ms()` to sample elapsed time without stopping the chrono.

### TD2
TD2 shows simple usage of POSIX timers and a CPU-consuming loop utility.

To answer the recurring TD2 question concerning the `const` qualifier on methods: add `const` to any method that should not modify the object's members. This helps protect class attributes from modification.

#### A — POSIX timer handler
A simple signal handler is installed and invoked periodically by a POSIX timer. The handler updates a value that the main program can read.

#### B — `Timer` class
`Timer` wraps a POSIX timer (created with `timer_create`) and exposes `start`, `start_ms` and `stop` methods. It requires derived classes to implement the virtual `callback()` which is called from the signal handler.

#### C — `Looper` class
`Looper` runs an empty loop for a specified number of iterations to consume CPU time.    
The class uses `volatile` for `doStop` and `iLoop` so the compiler does not optimize away or cache those variables; that ensures the loop can be stopped from another context.

#### D — `Calibrator`
`Calibrator` derives from `Timer` and samples the `Looper`'s progress at a fixed period. It collects samples, then performs a linear regression to compute coefficients $a$ and $b$ so that $nLoops(t) = a \times t + b$.    
The implementation starts the periodic timer and runs the looper until the desired number of samples is collected.

#### E — Busy-wait using calibrated loops
Using `Calibrator` and `Looper` together you can estimate the number of iterations required to occupy the CPU for a requested duration, then run that number of iterations to achieve approximate busy-waiting for that duration.   
To obtain a relative error below 10% for this target, we performed a denser calibration using 100 samples taken every 50 ms.

### TD3
TD3 demonstrates threads and mutexes usage.

#### A — Plain threads and counter
1) A single thread increments a local counter in a tight loop until it is signalled to stop. This produces the largest count per second ($\approx 3^9$ on local Ubuntu for 3 seconds) because there is no contention. 
2) Three threads increment a shared counter concurrently without synchronization. Due to race conditions, the final counter value is typically lower than the ideal sum of three independent counters. Increments may be lost so we divide the output by 10 ($\approx 3^8$ on local Ubuntu for 3 seconds).
3) The same three-thread test but with a `pthread_mutex_t` protecting the counter. The mutex serializes access to the counter, which eliminates lost increments but reduces throughput because threads wait for the lock. The output is again divided by 10 ($\approx 3^7$ on local Ubuntu for 3 seconds).

The exact numerical counter values depend heavily on the CPU and system load, so it could be different on another system (for example a Raspberry).

#### B — `Mutex` wrapper class
`Mutex` provides a small RAII-friendly wrapper around a POSIX mutex. Two nested classes are provided:
- `Mutex::Lock` — an RAII lock object. Constructing it locks the mutex, destroying it unlocks the mutex. A timed constructor is also available.
- `Mutex::TimeoutException` — thrown by the timed `Lock` constructor on timeout.

This design avoids forgetting to unlock the mutex (the destructor will unlock automatically if the lock object goes out of scope because it is on the stack).

#### C — `Thread` wrapper class
`Thread` provides a wrapper around POSIX threads.  

This helps us simplify thread usage for any function we want to run in a thread. Also, to simplify further we have defined static functions to choose a global policy because we want to prevent the use of multiple different policies which could cause problems.

#### D — `Counter` and `Incrementer`
`Counter` provides the possibility to launch a Counter with the counter value that could be _protected_ or not using a `Mutex` attribute.     
`Incrementer` derives from `Thread`, takes a `Counter` object reference and runs, as a thread, _nLoops_ iterations.   

What we want to see here is the real difference in terms of execution time and data protection between a simple thread and a thread using a mutex.     

I have done some experiments (on my personal PC) and here are my results:
|Protected|Policy|nLoops|nThreads|Execution Time (s)|Counter Value|Counter Value Expected|
|---|---|---:|---:|---:|---:|---:|
|No|SCHED_OTHER|1000|1|0.0|1000|1000|
|No|SCHED_OTHER|1000|3|0.0|3000|2983|
|No|SCHED_OTHER|1000|5|0.0|5000|4863|
|No|SCHED_OTHER|10⁸|1|0.5|10⁸|10⁸|
|No|SCHED_OTHER|10⁸|3|3.0|10.64⁸|30⁸|
|No|SCHED_OTHER|10⁸|5|4.9|20.10⁸|50⁸|
|Yes (mutex)|SCHED_OTHER|1000|1|0.0|1000|1000|
|Yes (mutex)|SCHED_OTHER|1000|3|0.0|3000|3000|
|Yes (mutex)|SCHED_OTHER|1000|5|0.0|5000|5000|
|Yes (mutex)|SCHED_OTHER|10⁸|1|6.3|10⁸|10⁸|
|Yes (mutex)|SCHED_OTHER|10⁸|3|85.8|30⁸|30⁸|
|Yes (mutex)|SCHED_OTHER|10⁸|5|206.1|50⁸|50⁸|
|Yes (mutex)|SCHED_RR|1000|1|0.0|1000|1000|
|Yes (mutex)|SCHED_RR|1000|3|0.0|3000|3000|
|Yes (mutex)|SCHED_RR|1000|5|0.0|5000|5000|
|Yes (mutex)|SCHED_RR|10⁸|1|6.3|10⁸|10⁸|
|Yes (mutex)|SCHED_RR|10⁸|3|86.8|30⁸|30⁸|
|Yes (mutex)|SCHED_RR|10⁸|5|209.7|50⁸|50⁸|
|Yes (mutex)|SCHED_FIFO|1000|1|0.0|1000|1000|
|Yes (mutex)|SCHED_FIFO|1000|3|0.0|3000|3000|
|Yes (mutex)|SCHED_FIFO|1000|5|0.0|5000|5000|
|Yes (mutex)|SCHED_FIFO|10⁸|1|9.4|10⁸|10⁸|
|Yes (mutex)|SCHED_FIFO|10⁸|3|72.1|30⁸|30⁸|
|Yes (mutex)|SCHED_FIFO|10⁸|5|201.9|50⁸|50⁸|

<small>Note: If you want to launch `./bin/TD3_d` you'll need to specify at least two parameters: `--nLoops x` and `--nTasks x`. You can also specify `--protect` to use mutexes and `--policy <policy>` to specify the policy you want to use. </small>
<small>Note bis: You need to be _sudo_ to launch correctly SCHED_FIFO and SCHED_RR.</small>

As we can see, with only one task, running without protection is faster and the result is equivalent, which is normal because there is no other thread trying to use the same value. But with more than one thread, the value is no longer equal to the expected one and is in fact lower because the value can be overwritten by another thread.

Also, we can see that when we are using mutexes, we have no problems getting the correct value, but when there are many loops to execute, there is a significant time difference compared to the non-protected version because acquiring and releasing the lock add some overhead each time we modify the counter value.

#### E — Priority Inversion
In this part we modified the `Mutex` class to enable priority inheritance (to mitigate priority inversion).

With priority inheritance, when a high-priority task needs a resource held by a lower-priority task, the lower-priority task temporarily inherits the higher priority so it can finish and release the resource sooner.

We define three tasks, A, B and C, with different priority levels. C starts and acquires the resource, but is then suspended to allow B and A to start because they have higher priorities. A cannot acquire the resource while C holds it, so A is blocked and B continues. With priority inheritance, C should inherit A's priority, finish its critical section and release the mutex sooner, which reduces A's waiting time.

I have done some experiments with and without priority inheritance on the protected resource. Each _tick_ is about 10 ms and I ensured only one CPU was active so the Raspberry Pi OS did not migrate tasks between cores. Here are the results:

Results without priority inheritance (resource unprotected):

|Task|Priority|Execution Time (ms)|
|---|---|---:|
|C|Low|10519.0|
|A|High|6463.0|
|B|Medium|2109.0|

Results with priority inheritance (resource protected):

|Task|Priority|Execution Time|
|---|---|---:|
|C|Low|10583.0|
|A|High|5471.0|
|B|Medium|6527.0|

<small>Note: These tests were performed on a Raspberry Pi.</small>

To do this test, please launch the commands below: 
```
scp -r . <your-dev-login>@147.250.8.198:Dir_name
<Connect to the Raspberry Pi>
cd Dir_name
arm-linux-g++ -std=c++11 -O3 -Iinclude -Wall -Wextra testbench/TD3_e.cpp src/calibrator.cpp src/chrono.cpp src/counter.cpp src/cpu_loop.cpp src/incrementer.cpp src/looper.cpp src/mutex.cpp src/thread.cpp src/timer.cpp src/timespec_utils.cpp -o TD3_e -lrt -pthread
rsync -avz TD3_e root@192.168.50.xx:
ssh root@192.168.50.xx
./TD3_e
```