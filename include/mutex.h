#pragma once

#include <pthread.h>
#include <string>
#include <thread>

namespace donkeycar {
namespace concurrency {

// A mutex based on pthread_mutex_t
class Mutex {

public:
    // Force usage of the ctor taking a string name
    Mutex() = delete;
    explicit Mutex(std::string name);
    ~Mutex();

    // Disable copy and move construction, and move assignment operations
    Mutex(const Mutex &) = delete;
    Mutex(Mutex &&) = delete;
    Mutex &operator=(const Mutex &) = delete;
    Mutex &operator=(Mutex &&) = delete;

    void lock();
    void unlock();

    bool try_lock();
    pthread_mutex_t& underlying_handle();

protected:
    pthread_mutex_t m_mutex{};
    std::string m_name;
};

} // namespace concurrency 
} // namespace donkeycar 
