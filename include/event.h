#pragma once

#include <pthread.h>
#include <stdint.h>

namespace donkeycar {
namespace concurrency {

// Event is effectively a condition-variable based on pthread_cond_t. It
// supports a timeout wait based on timespec and the monotonic clock from
// time.h
class Mutex;
class Event {
   public:
    Event();
    ~Event();

    // disable copy-construction and move-construction operations
    Event(const Event &) = delete;
    Event(const Event &&) = delete;

    // disable copy-assignment and move-assignment operations
    Event &operator=(const Event &) = delete;
    // move assignment operator
    // The && indicates an "rvalue reference"
    // An rvalue reference can bind to an rvalue (but not to an lvalue)
    Event &operator=(Event &&) = delete;

    void trigger_all();
    void wait(donkeycar::concurrency::Mutex &mutex);
    bool wait(donkeycar::concurrency::Mutex &mutext, unsigned int timeout_ms);

   private:
    timespec get_monotonic_clock_time();
    timespec ms_to_timespec(const uint64_t milliseconds);

    pthread_cond_t m_cond{};
};
timespec operator+(const timespec &, const timespec &);

}  // namespace concurrency
}  // namespace donkeycar
