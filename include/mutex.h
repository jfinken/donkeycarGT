#pragma once

#include <pthread.h>
#include <string>
#include <thread>

namespace donkeycar {
namespace concurrency {

/**
 * @brief A mutex based on pthread_mutex_t
 *
 */
class Mutex {
   public:
    /**
     * @brief Construct a new Mutex object
     * Force usage of the ctor taking a string name
     * @param name
     */
    explicit Mutex(std::string name);
    Mutex() = delete;
    ~Mutex();

    /**
     * @brief As customary with mutex objects, disable copy and move
     * construction, and move assignment operations
     */
    Mutex(const Mutex &) = delete;
    Mutex(Mutex &&) = delete;
    Mutex &operator=(const Mutex &) = delete;
    Mutex &operator=(Mutex &&) = delete;

    /**
     * @brief Lock the underlying mutex
     */
    void lock();

    /**
     * @brief Unlock the underlying mutex
     */
    void unlock();

    /**
     * @brief Try locking the mutex. Return true if successful, false otherwise
     *
     * @return true
     * @return false
     */
    bool try_lock();

    /**
     * @brief Provide access to the underlying pthread_mutex
     *
     * @return pthread_mutex_t&
     */
    pthread_mutex_t &underlying_handle();

   protected:
    pthread_mutex_t m_mutex{};
    std::string m_name;
};

}  // namespace concurrency
}  // namespace donkeycar
