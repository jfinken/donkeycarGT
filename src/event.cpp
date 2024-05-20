#include "event.h"
#include <time.h>
#include <cassert>
#include "mutex.h"

const long NS_PER_SECOND = 1000000000L;
const long NS_PER_MS = 1000000L;

namespace donkeycar {
namespace concurrency {

Event::Event() {
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_cond_init(&m_cond, &attr);
    pthread_condattr_destroy(&attr);
}

Event::~Event() { pthread_cond_destroy(&m_cond); }

void Event::trigger_all() { pthread_cond_signal(&m_cond); }

void Event::wait(donkeycar::concurrency::Mutex& mutex) {
    pthread_cond_wait(&this->m_cond, &mutex.underlying_handle());
}

bool Event::wait(donkeycar::concurrency::Mutex& mutex,
                 unsigned int timeout_ms) {
    const timespec start = get_monotonic_clock_time(),
                   wait = ms_to_timespec(timeout_ms), timeout = start + wait;

    return pthread_cond_timedwait(&this->m_cond, &mutex.underlying_handle(),
                                  &timeout) == 0;
}

timespec Event::get_monotonic_clock_time() {
    timespec out;
    int ret = clock_gettime(CLOCK_MONOTONIC, &out);
    assert(ret == 0);
    (void)ret;
    return out;
}
timespec Event::ms_to_timespec(const uint64_t milliseconds) {
    timespec out;
    const uint64_t ns = milliseconds * NS_PER_MS;
    out.tv_nsec = ns % NS_PER_SECOND;
    out.tv_sec = ns / NS_PER_SECOND;
    return out;
}
timespec operator+(const timespec& lhs, const timespec& rhs) {
    // Enforce assumption on positive values.
    assert(lhs.tv_sec >= 0 and rhs.tv_sec >= 0);

    timespec out;
    const uint64_t ns = lhs.tv_nsec + rhs.tv_nsec;
    out.tv_nsec = ns % NS_PER_SECOND;
    out.tv_sec = lhs.tv_sec + rhs.tv_sec + ns / NS_PER_SECOND;
    return out;
}
}  // namespace concurrency
}  // namespace donkeycar
