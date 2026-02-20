#pragma once

#include <pthread.h>
#include "chrono.h"

class Thread 
{
    private:
        pthread_t posixThreadId;
        pthread_attr_t posixThreadAttrId;

        int schedPolicy;
        bool isThreadStarted;

        Chrono chrono;

        static void* call_run(void* v_Thread);

    protected:
        virtual void run() = 0; // We are exposing this as an interface (virtual), we don't want the user (or another thread) to launch it (why it is not public)

    public:
        const int id; // To be able to identify which id in printing for example

        Thread(int _id_);

        virtual ~Thread();

        void start(int priority = 0);

        void join();

        bool isStarted();

        long duration_ms();

        static int getMaxPrio(int policy);

        static int getMinPrio(int policy);

        static void setMainSched(int policy);

        static int getMainSched();
};

