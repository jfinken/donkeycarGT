#pragma once

#include "event.h"
#include "mutex.h"

// Status of a task shared with its supervisor or application
// The intent here is that this class is shared between a 
// managed thread and its supervisor, either directly or via
// ManagedThread API.
//
// The supervising application may use this class to check the
// thread's running state and command it to pause/resume.
//
// The thread on the other hand uses this class to receive commands
// from and direct acknowledgement to the supervising application.
namespace donkeycar {
namespace concurrency {

class ManagedTaskStatus
{
public:
    ManagedTaskStatus(const std::string& task_name, bool running);
    void pause_task_blocking();
    void resume_task_blocking();
    void set_exit_flag_blocking();
    bool request_pause() const;
    bool wait_to_run();
    bool is_running() const;
    bool is_paused() const;

private:
    // state machine for the thread or task
    enum ManagedTaskStatusStateMachine {
        MTSSM_RUNNING = 0,
        MTSSM_REQUEST_PAUSE = 1,
        MTSSM_PAUSED = 2,
        MTSSM_REQUEST_RUN = 3,
        MTSSM_REQUEST_EXIT = 4,
    };
    mutable donkeycar::concurrency::Mutex m_mutex{"ManagedTaskStatus"};
    donkeycar::concurrency::Event m_event;
    // volatile: this variable may change "rapidly and unpredictably". Thus tell
    // the compiler to no longer optimize the code, and to provide stable access
    // to the original memory address.
    volatile ManagedTaskStatusStateMachine m_state;
    const std::string m_task_name;
};

} // namespace concurrency 
} // namespace donkeycar 

