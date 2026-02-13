#pragma once
#include <time.h>

#include "timespec_utils.h"

class Chrono
{
private:
    timespec m_duration;

public:
    timespec m_startTime;
    timespec m_stopTime;

    Chrono();

    void restart();

    timespec stop();

    bool isActive() const;

    timespec lap() const;

    double lap_ms() const;
};