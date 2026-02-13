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
2) Three threads increment a shared counter concurrently without synchronization. Due to race conditions, the final counter value is typically lower than the ideal sum of three independent counters.Increments may be lost so we devide by $10$ the output ($\approx 3^8$ on local Ubuntu for 3 seconds).
3) The same three-thread test but with a `pthread_mutex_t` protecting the counter. The mutex serializes access to the counter, which eliminates lost increments but reduces throughput because threads wait for the lock. The ouput is again devided by 10 ($\approx 3^7$ on local Ubuntu for 3 seconds).

The exact numerical counter values depend heavily on the CPU and system load, so it could be different on another system (for example a Raspberry).

#### B — `Mutex` wrapper class
`Mutex` provides a small RAII-friendly wrapper around a POSIX mutex. Two nested classes are provided:
- `Mutex::Lock` — an RAII lock object. Constructing it locks the mutex, destroying it unlocks the mutex. A timed constructor is also available.
- `Mutex::TimeoutException` — thrown by the timed `Lock` constructor on timeout.

This design avoids forgetting to unlock the mutex (the destructor will unlock automatically if the lock object goes out of scope because it is on the stack).
