
#include "managed-task-status.h"
#include <cassert>
#include "lock.hpp"

namespace donkeycar {
namespace concurrency {

namespace concurrency = donkeycar::concurrency;

ManagedTaskStatus::ManagedTaskStatus(const std::string& task_name, bool running)
    : m_mutex(task_name),
      m_event(),
      m_state(running ? MTSSM_RUNNING : MTSSM_PAUSED),
      m_task_name(task_name) {}

void ManagedTaskStatus::pause_task_blocking() {
    concurrency::Lock lock(m_mutex);
    assert(m_state != MTSSM_REQUEST_RUN);
    assert(m_state != MTSSM_REQUEST_EXIT);
    if (m_state == MTSSM_PAUSED) {
        return;
    }
    // state must currently be running
    m_state = MTSSM_REQUEST_PAUSE;
    m_event.trigger_all();

    // wait until it is paused
    while (m_state != MTSSM_PAUSED) {
        // printf("Waiting for %s task to pause\n", m_task_name.c_str());
        m_event.wait(m_mutex, 1000);
    }
}

void ManagedTaskStatus::resume_task_blocking() {
    concurrency::Lock lock(m_mutex);
    assert(m_state != MTSSM_REQUEST_PAUSE);
    assert(m_state != MTSSM_REQUEST_EXIT);
    if (m_state == MTSSM_RUNNING)  // already running
    {
        return;
    }
    // now state must be MTSSM_PAUSED
    m_state = MTSSM_REQUEST_RUN;
    m_event.trigger_all();
    // wait until it is resumed
    while (m_state != MTSSM_RUNNING) {
        // printf("Waiting for %s task to resume\n", m_task_name.c_str());
        m_event.wait(m_mutex, 100);
    }
    printf("%s task resumed\n", m_task_name.c_str());
}

bool ManagedTaskStatus::request_pause() const {
    concurrency::Lock lock(m_mutex);
    return (m_state == MTSSM_REQUEST_PAUSE);
}

bool ManagedTaskStatus::wait_to_run() {
    concurrency::Lock lock(m_mutex);
    // if a controller is not interfering then skip
    if (m_state == MTSSM_RUNNING) {
        return true;
    }
    // if requested to exit we cannot run anymore
    if (m_state == MTSSM_REQUEST_EXIT) {
        // printf("%s task exiting\n", m_task_name.c_str());
        return false;
    }
    // if already paused or a controller wants to resume then skip
    if (m_state == MTSSM_REQUEST_PAUSE) {
        // printf("%s task pausing\n", m_task_name.c_str());
        m_state = MTSSM_PAUSED;
        m_event.trigger_all();  // inform the controller
        return false;
    }
    // wait until a controller wants to resume
    while (m_state != MTSSM_REQUEST_RUN) {
        m_event.wait(m_mutex, 100);
        if (m_state == MTSSM_REQUEST_EXIT) {
            // printf("%s task asked to exit\n", m_task_name.c_str());
            return false;
        }
    }
    // printf("%s task resuming\n", m_task_name.c_str());
    m_state = MTSSM_RUNNING;
    m_event.trigger_all();  // inform the controller
    return true;
}

void ManagedTaskStatus::set_exit_flag_blocking() {
    concurrency::Lock lock(m_mutex);
    // printf("%s task marked to exit\n", m_task_name.c_str());
    m_state = MTSSM_REQUEST_EXIT;
    m_event.trigger_all();  // inform the controller
}

bool ManagedTaskStatus::is_running() const {
    concurrency::Lock lock(m_mutex);
    return m_state == MTSSM_RUNNING;
}
bool ManagedTaskStatus::is_paused() const {
    concurrency::Lock lock(m_mutex);
    return m_state == MTSSM_PAUSED;
}

}  // namespace concurrency
}  // namespace donkeycar
