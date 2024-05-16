
#include "mutex.h"
#include <cerrno>

namespace donkeycar {
namespace concurrency {

    Mutex::Mutex(std::string name) : m_name(std::move(name))
    {
        // Initialize the underlying ptread_mutex
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_mutex, &attr);
        pthread_mutexattr_destroy(&attr);
    }
    Mutex::~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    void Mutex::lock()
    {
        pthread_mutex_lock(&m_mutex);
    }
    void Mutex::unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

    bool Mutex::try_lock()
    {
        int ret = pthread_mutex_trylock(&m_mutex);
        if(ret == EBUSY)
            return false;
        else 
            return true;
    }

    pthread_mutex_t& Mutex::underlying_handle() 
    {
        return m_mutex;
    }
} // namespace concurrency 
} // namespace donkeycar 
