#include "../include/chrono.h"

Chrono::Chrono() 
{
    restart();
}

void Chrono::restart() 
{
    m_startTime = timespec_now();
    m_stopTime = m_startTime;
}

timespec Chrono::stop() 
{
    m_stopTime = timespec_now();
    m_duration = m_stopTime - m_startTime;
    return m_duration;
}

bool Chrono::isActive() const 
{
    return m_startTime == m_stopTime;
}

timespec Chrono::lap() const
{
    if (isActive()) 
    {
        timespec now = timespec_now();
        timespec lapDuration = now - m_startTime;
        return lapDuration;
    } 
    else 
    {
        return m_duration;
    }
}

double Chrono::lap_ms() const
{
    return timespec_to_ms(lap());
}
