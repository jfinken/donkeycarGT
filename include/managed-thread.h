#pragma once

#include <atomic>
#include <thread>
#include "lock.hpp"
#include "managed-task-status.h"
#include "mutex.h"

namespace donkeycar {
namespace concurrency {

// A class wrapping std::thread, ManagedThread, provides concurrency with
// management semantics.  A controlling class or application manages the
// thread via this API, under the hood the thread is started, paused,
// resumed, and joined via a locked and condition-variable-based state machine.
class ManagedThread {
   public:
    ManagedThread(const std::string &thread_name, bool start_paused);
    virtual ~ManagedThread();

    // Disable copy-construction, assignment and move operations
    ManagedThread(const ManagedThread &) = delete;
    ManagedThread &operator=(const ManagedThread &) = delete;
    // typically come in pairs: move constructor, move assignment
    ManagedThread(ManagedThread &&) = delete;
    ManagedThread &operator=(ManagedThread &&) = delete;

    virtual void start();
    void join();
    void resume();
    void suspend();
    void graceful_exit();
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
}  // namespace concurrency
}  // namespace donkeycar