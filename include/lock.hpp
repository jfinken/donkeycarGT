#pragma once

#include <cassert>
#include "mutex.h"

namespace donkeycar::concurrency {
/**
 * @brief Lock implements RAII-style (Resource Allocation Is Initialization)
 * acquisition of a Mutex. The basic idea is to represent a resource by a
 * local object, so that the local object’s destructor will release the
 * resource.
 *
 * When a Lock is constructed, the mutex passed to its constructor
 * is taken.  When it is destroyed, the mutex is released.  The mutex is held
 * by the creator for the lifetime of the Lock object.  This scheme makes it
 * impossible to fail to unlock a mutex, even with exceptions.
 */
class Lock {
   public:
    /**
     * @brief Construct a new Lock object and lock the mutex for the lifetime of
     * the lock
     *
     * @param mutex
     */
    explicit Lock(Mutex &mutex) : m_mutex(&mutex), m_lock_cnt(1) {
        m_mutex->lock();
    }
    Lock() : m_mutex(nullptr), m_lock_cnt(0) {}

    /**
     * @brief Construct a new Lock object
     * Copy-construction is identical to creating a new lock from the same mutex
     * @param other
     */
    Lock(const Lock &other) : Lock(*other.m_mutex) {}

    /**
     * @brief Copy-assignment is prohibited to prevent long-lived locks which
     * change state many times.
     */
    Lock &operator=(const Lock &) = delete;

    /**
     * @brief Move-Construct a new Lock object (from an rvalue)
     *
     * @param other
     */
    Lock(Lock &&other) : m_mutex(nullptr), m_lock_cnt(0) {
        *this = std::move(other);
    }

    /**
     * @brief Assignment-operator support
     * @param other
     * @return Lock&
     */
    Lock &operator=(Lock &&other) {
        std::swap(m_mutex, other.m_mutex);
        std::swap(m_lock_cnt, other.m_lock_cnt);
        return *this;
    }

    /**
     * @brief Unlock the mutex associated with this lock. RAII-style, like defer
     * in Golang
     */
    ~Lock() {
        while (m_lock_cnt > 0) {
            unlock();
        }
    }

   private:
    /**
     * @brief Don't explicitly call this, just destroy and create another when
     * you want to take the mutex
     */
    void unlock() {
        assert(m_mutex != nullptr);
        m_mutex->unlock();
        --m_lock_cnt;
    }

    Mutex *m_mutex;
    int m_lock_cnt;
};
}  // namespace donkeycar::concurrency
