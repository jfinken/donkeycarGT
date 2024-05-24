#pragma once

#include <atomic>
#include <thread>
#include "lock.hpp"
#include "managed-task-status.h"
#include "mutex.h"

namespace donkeycar::concurrency {

/**
 * @brief Class wrapping std::thread, ManagedThread, provides concurrency with
 * overlord management semantics.  A controlling class or application manages
 * the thread via this API, under the hood the thread is started, paused,
 * resumed, and joined via a locked and condition-variable-based state machine.
 */
class ManagedThread {
   public:
    /**
     * @brief Construct a new Managed Thread object
     *
     * @param thread_name [in] assigned to the thread for logging, etc
     * @param start_paused [in] whether or not to create the thread in the
     * paused state
     */
    ManagedThread(const std::string &thread_name, bool start_paused);
    virtual ~ManagedThread();

    /**
     * @brief Disable copy-construction, assignment and move operations
     */
    ManagedThread(const ManagedThread &) = delete;
    ManagedThread &operator=(const ManagedThread &) = delete;
    // typically come in pairs: move constructor, move assignment
    ManagedThread(ManagedThread &&) = delete;
    ManagedThread &operator=(ManagedThread &&) = delete;

    /**
     * @brief Create the thread. However if start_paused == true, then
     * the thread will be running but will be blocked on the cv-based TaskStatus
     * state machine.  Hence, managed.
     */
    virtual void start();

    /**
     * @brief Shutdown the thread, if joinable
     */
    void join();

    /**
     * @brief Resume the (typically) paused thread
     */
    void resume();

    /**
     * @brief Pause the thread
     */
    void suspend();

    /**
     * @brief Tell the state machine mgr to *exit* the main work loop of the
     * thread
     */
    void graceful_exit();

    /**
     * @brief Return true if this thread is paused
     *
     * @return true
     * @return false
     */
    bool is_paused() const;

   protected:
    bool is_thread_started() const { return m_thread.joinable(); }
    // this is the thing that controls the thread
    donkeycar::concurrency::ManagedTaskStatus m_task_status;

   private:
    // this is the main loop
    void task();
    // function executed in the main loop
    virtual void update() = 0;

    virtual bool should_exit() const { return false; }

    std::thread m_thread;
    std::atomic<bool> m_should_exit{false};
    donkeycar::concurrency::Mutex m_mutex_pause_resume;
};
}  // namespace donkeycar::concurrency