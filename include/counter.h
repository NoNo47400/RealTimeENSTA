#pragma once

#include "mutex.h"

class Counter
{
    private:
        Mutex* p_mutex; // Also possible to use smartpointer
        bool is_protected;
        double m_value;

    public:
        Counter(int _is_protected_);

        ~Counter();

        bool isProtected() const;

        double increment();

        double getValue() const;
};