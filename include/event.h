#pragma once

#include <pthread.h>
#include <stdint.h>

namespace donkeycar::concurrency {

class Mutex;
/**
 * @brief Event is effectively a condition-variable based on pthread_cond_t. It
 * supports a timeout wait based on timespec and the monotonic clock from time.h
 */
class Event {
   public:
    /**
     * @brief Construct a new Event object but disable copy-construction and
     * move-construction operations
     */
    Event();
    ~Event();
    Event(const Event &) = delete;
    Event(const Event &&) = delete;

    /**
     * @brief Disable copy-assignment and move-assignment operations
     */
    Event &operator=(const Event &) = delete;
    // Note the && indicates an rvalue reference
    // An rvalue reference can bind to an rvalue (but not to an lvalue)
    Event &operator=(Event &&) = delete;

    /**
     * @brief Wake up an underlying thread waiting on the condition-variable
     */
    void trigger_all();
    /**
     * @brief Wait for the underlying condition-variable to be triggered or
     * broadcast
     * @param mutex
     */
    void wait(donkeycar::concurrency::Mutex &mutex);
    /**
     * @brief Wait for the underlying condition-variable to be triggered or
     * broadcast until timeout in milliseconds.
     * @param mutex
     * @param timeout_ms
     */
    bool wait(donkeycar::concurrency::Mutex &mutext, unsigned int timeout_ms);

   private:
    // Get and return the value of the underlying monotonic clock
    timespec get_monotonic_clock_time();
    // Convert the input milliseconds to a timespec object
    timespec ms_to_timespec(const uint64_t milliseconds);

    pthread_cond_t m_cond{};
};
timespec operator+(const timespec &, const timespec &);

}  // namespace donkeycar::concurrency
