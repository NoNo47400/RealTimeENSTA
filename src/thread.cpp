#include "../include/thread.h"

int Thread::getMaxPrio(int policy)
{
    return sched_get_priority_max(policy);
}

int Thread::getMinPrio(int policy)
{
    return sched_get_priority_min(policy);
}

void Thread::setMainSched(int policy)
{
    sched_param schedParam;
    schedParam.sched_priority = getMaxPrio(policy);
    pthread_setschedparam(pthread_self(), policy, &schedParam); // pthread_self() gives us the id of the thread we are calling
}

int Thread::getMainSched()
{
    sched_param schedParam; // Need to define it to not have a warning about it being null
    int schedPolicy;
    pthread_getschedparam(pthread_self(), &schedPolicy, &schedParam);
    return schedPolicy;
}

void* Thread::call_run(void* v_Thread) // We need to return a void* because it is the signature waited
{
    Thread* thread = static_cast<Thread*>(v_Thread);
    thread->run();
    thread->chrono.stop();
    thread->isThreadStarted = false;
    return thread;
}

void Thread::join() const
{
    pthread_join(posixThreadId, NULL);
}

Thread::Thread(int _id_) : id(_id_)
{
    pthread_attr_init(&posixThreadAttrId);
}

Thread::~Thread()
{
    pthread_attr_destroy(&posixThreadAttrId);
}

bool Thread::isStarted() const
{
    return isThreadStarted;
}

long Thread::duration_ms() const
{
    return long(chrono.lap_ms());
}

void Thread::start(int priority)
{
    sched_param schedParam; // Need to define it to not have a warning about it being null
    int schedPolicy;
    pthread_getschedparam(pthread_self(), &schedPolicy, &schedParam);

    if (schedPolicy == SCHED_OTHER)
    {   
        priority = 0;
    }
    else
    {
        if (priority > 99)
        {
            priority = getMaxPrio(schedPolicy);
        }
        else if (priority < 1)
        {
            priority = getMinPrio(schedPolicy);
        }
    }

    pthread_create(&posixThreadId, &posixThreadAttrId, call_run, this);

    schedParam.sched_priority = priority;
    pthread_setschedparam(posixThreadId, schedPolicy, &schedParam);

    chrono.restart();
    isThreadStarted = true;
}