#include "../include/counter.h"

Counter::Counter(int _is_protected_) : is_protected(_is_protected_)
{
    if (is_protected) // It is possible to use an std::function with incrDoProtect and incrNoProtect with a lambda to simplify
    {
        p_mutex = new Mutex();
    }
    else
    {
        p_mutex = nullptr;
    }
    m_value = 0.0;  
}

Counter::~Counter()
{
    delete p_mutex;
}

bool Counter::isProtected() const
{
    return is_protected;
}

double Counter::increment()
{
    if (is_protected) // It is possible to use an std::function with incrDoProtect and incrNoProtect with a lambda to simplify
    {
        try
        {
            Mutex::Lock lock(*p_mutex, 10.0);
            m_value += 1.0; 
            return m_value;

        }
        catch (const Mutex::TimeoutException& e)
        {
            std::cerr << "TimeoutException: Delayed execution exceeded: " << e.timeout_ms << " ms" << std::endl;
        }
    }
    m_value += 1.0;
    return m_value;
}

double Counter::getValue() const
{
    if (is_protected) // It is possible to use an std::function with incrDoProtect and incrNoProtect with a lambda to simplify
    {
        try
        {
            Mutex::Lock lock(*p_mutex, 10.0);
            return m_value;
        }
        catch (const Mutex::TimeoutException& e)
        {
            std::cerr << "TimeoutException: Delayed execution exceeded: " << e.timeout_ms << " ms" << std::endl;
        }
    }
    return m_value;
}

