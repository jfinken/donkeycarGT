#pragma once

#include "lock.hpp"
#include "mutex.h"
#include "managed-task-status.h"
#include <atomic>
#include <thread>

namespace donkeycar {
namespace concurrency {

// TODO: document me
class ManagedThread
{

public:
    ManagedThread(const std::string& thread_name, bool start_paused);
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
} // namespace concurrency
} // namespace donkeycar