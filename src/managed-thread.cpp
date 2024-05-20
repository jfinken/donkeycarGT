#include "managed-thread.h"

#include <functional>

namespace donkeycar {
namespace concurrency {

namespace concurrency = donkeycar::concurrency;

ManagedThread::ManagedThread(const std::string& thread_name, bool start_paused)
    : m_task_status(thread_name, !start_paused),
      m_mutex_pause_resume(thread_name) {}

ManagedThread::~ManagedThread() {
    graceful_exit();
    join();
}

void ManagedThread::task() {
    // should_exit() can be implemented by the derived class
    while (!m_should_exit) {
        // should we be exiting
        if (!m_should_exit && should_exit()) {
            graceful_exit();
        }
        // management: wait_to_run can be blocking on a CV, triggered
        // by a supervision application
        bool should_run = m_task_status.wait_to_run();

        if (should_run && !m_should_exit && !should_exit()) {
            update();
        }
    }
}

void ManagedThread::resume() {
    concurrency::Lock lock(m_mutex_pause_resume);
    m_task_status.resume_task_blocking();
}

void ManagedThread::suspend() {
    concurrency::Lock lock(m_mutex_pause_resume);
    m_task_status.pause_task_blocking();
}

void ManagedThread::graceful_exit() {
    m_should_exit = true;
    // resume so it can actually exit, else we'll hang
    concurrency::Lock lock(m_mutex_pause_resume);
    m_task_status.set_exit_flag_blocking();
}

bool ManagedThread::is_paused() const { return m_task_status.is_paused(); }

void ManagedThread::start() {
    concurrency::Lock lock(m_mutex_pause_resume);
    m_should_exit = false;
    assert(!m_thread.joinable() && "[start] managed-thread already started!");

    // the thread starts to execute after it is created.
    m_thread = std::thread(std::bind(&ManagedThread::task, this));
}

void ManagedThread::join() {
    concurrency::Lock lock(m_mutex_pause_resume);
    if (m_thread.joinable()) {
        m_thread.join();
    }
}
}  // namespace concurrency
}  // namespace donkeycar