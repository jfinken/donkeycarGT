#pragma once

#include "event.h"
#include "mutex.h"

namespace donkeycar::concurrency {

/**
 * @brief Status of a task shared with a supervisor, application or overlord.
 * The intent here is that this class is shared between a managed thread and its
 * managing app, either directly or via ManagedThread API. The supervising
 * application may use this class to check the thread's running state and
 * command it to pause/resume. The thread on the other hand uses this class to
 * receive commands from and direct acknowledgement to the supervising
 * application.
 */
class ManagedTaskStatus {
   public:
    /**
     * @brief Construct a new Managed Task Status object
     *
     * @param task_name [in] string name assigned to the thread
     * @param running [in] whether to start the thread in the running state (vs
     * paused)
     */
    ManagedTaskStatus(const std::string& task_name, bool running);

    /**
     * @brief Blocking call to pause a managed thread
     */
    void pause_task_blocking();

    /**
     * @brief Blocking call to resume a managed thread
     */
    void resume_task_blocking();

    /**
     * @brief Blocking call to exit a managed thread
     */
    void set_exit_flag_blocking();

    /**
     * @brief Non-blocking call to pause a managed thread
     */
    bool request_pause() const;

    /**
     * @brief Main workhorse of this class.  Threads under management will
     * call this method and, if not already running, will block and wait
     *
     * @return true
     * @return false
     */
    bool wait_to_run();

    /**
     * @brief Return true if in the running state, false otherwise
     *
     * @return true
     * @return false
     */
    bool is_running() const;

    /**
     * @brief Return true if in the paused state, false otherwise
     *
     * @return true
     * @return false
     */
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

}  // namespace donkeycar::concurrency
