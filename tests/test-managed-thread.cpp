#include <chrono>
#include <iostream>
#include <thread>

#include "doctest/doctest.h"
#include "managed-thread.h"

namespace concurrency = donkeycar::concurrency;

static bool task_executed;
static bool dtor_ran;
static concurrency::Mutex test_mutex("test-mutex");
static concurrency::Event test_event;

class TestThread : public concurrency::ManagedThread {
   public:
    TestThread(bool start_paused)
        : ManagedThread("test-thread", start_paused),
          task_exec_time_ms(0),
          test_thread_should_exit(false) {
        task_executed = false;
        dtor_ran = false;
    }

    bool wait_for_task_to_run() {
        concurrency::Lock lock(test_mutex);
        bool success = test_event.wait(test_mutex, 500);
        return success;
    }

    ~TestThread() {
        std::cout << "[~doctest-test-thread dtor]" << std::endl;
        dtor_ran = true;

        graceful_exit();
        join();
    }

    int task_exec_time_ms;
    std::atomic<bool> test_thread_should_exit;

   private:
    void update() override {
        std::cout << "[doctest-test-thread] update enter" << std::endl;
        // do some work...
        std::this_thread::sleep_for(
            std::chrono::milliseconds(task_exec_time_ms));

        concurrency::Lock lock(test_mutex);
        task_executed = true;
        std::cout << "[doctest-test-thread] update trigger_all" << std::endl;
        test_event.trigger_all();
        std::cout << "[doctest-test-thread] update exit" << std::endl;
    }
    bool should_exit() const override { return test_thread_should_exit; }
};

TEST_CASE("TestThread: start usecases") {
    // Not really a good use, but exploring SUBCASE

    SUBCASE("TestThread: start_paused") {
        TestThread t(true);
        CHECK(t.is_paused() == true);

        t.start();
        CHECK(t.is_paused() == true);
        CHECK(task_executed == false);

        t.resume();
        CHECK(t.is_paused() == false);

        t.graceful_exit();
        t.join();
    }
    SUBCASE("TestThread: start_running") {
        TestThread t(false /*start_paused=false*/);
        CHECK(t.is_paused() == false);

        t.start();
        CHECK(t.is_paused() == false);
        t.suspend();
        CHECK(t.is_paused() == true);

        t.resume();
        CHECK(t.is_paused() == false);

        t.graceful_exit();
        t.join();
    }
}

TEST_CASE("TestThread: exit usecases") {
    SUBCASE("TestThread: exit_not_started_paused") {
        TestThread t(true);
        CHECK(t.is_paused() == true);
        t.graceful_exit();
        t.join();

        // update should not have executed
        CHECK(task_executed == false);
    }
    SUBCASE("TestThread: exit_not_started_running") {
        TestThread t(false);
        CHECK(t.is_paused() == false);
        t.graceful_exit();
        t.join();
        // update should not have executed
        CHECK(task_executed == false);
    }
    SUBCASE("TestThread: exit_started_paused") {
        TestThread t(true);
        CHECK(t.is_paused() == true);
        t.start();
        CHECK(t.is_paused() == true);
        t.graceful_exit();
        t.join();

        // we started paused, update should not have executed
        CHECK(task_executed == false);
    }
}

TEST_CASE("TestThread: exit_after_pause") {
    TestThread t(false);
    CHECK(t.is_paused() == false);
    t.start();

    CHECK(t.wait_for_task_to_run() == true);
    t.suspend();
    CHECK(t.is_paused() == true);

    // suspend blocks and waits, task should have executed...
    CHECK(task_executed == true);

    // during the graceful exit, update should not be executing
    task_executed = false;
    t.graceful_exit();
    t.join();
    CHECK(task_executed == false);
}

// I mean much of this is testing that we shutdown and exited
// gracefully.  Otherwise we could declare the thread outside of
// the SUBSCASE scope and reuse t.
TEST_CASE("TestThread: tests with execution time") {
    SUBCASE("exit_paused_with_execution_time") {
        TestThread t(false);
        t.task_exec_time_ms = 100;
        CHECK(t.is_paused() == false);
        t.start();
        CHECK(t.is_paused() == false);
        CHECK(t.wait_for_task_to_run() == true);

        t.suspend();

        // during the graceful exit, update should not be executing
        task_executed = false;
        t.graceful_exit();
        t.join();
        CHECK(task_executed == false);
    }
    SUBCASE("exit-running-with-exec-time-and-wait") {
        TestThread t(false);
        t.task_exec_time_ms = 100;
        CHECK(t.is_paused() == false);
        t.start();
        CHECK(t.is_paused() == false);
        CHECK(t.wait_for_task_to_run() == true);

        t.suspend();
        CHECK(t.is_paused() == true);

        // the task should have executed
        CHECK(task_executed == true);

        t.resume();
        CHECK(t.is_paused() == false);
        CHECK(t.wait_for_task_to_run() == true);
        t.graceful_exit();
        t.join();
    }
    SUBCASE("exit-running-using-dtor-to-cleanup") {
        {
            TestThread t(false);
            t.task_exec_time_ms = 100;
            CHECK(t.is_paused() == false);
            t.start();
            CHECK(t.is_paused() == false);
            CHECK(t.wait_for_task_to_run() == true);

            t.suspend();
            CHECK(t.is_paused() == true);

            // the task should have executed
            CHECK(task_executed == true);

            t.resume();
            CHECK(t.is_paused() == false);
        }
        // can't explicitly call and test the dtor
        CHECK(dtor_ran == true);
    }
    SUBCASE("exit-paused-using-dtor-to-cleanup") {
        {
            TestThread t(false);
            t.task_exec_time_ms = 100;
            CHECK(t.is_paused() == false);
            t.start();
            CHECK(t.is_paused() == false);
            CHECK(t.wait_for_task_to_run() == true);

            t.suspend();
            CHECK(t.is_paused() == true);

            // the task should have executed
            CHECK(task_executed == true);

            task_executed = false;
        }
        // can't explicitly call and test the dtor
        CHECK(dtor_ran == true);
        CHECK(task_executed == false);
    }
    SUBCASE("exit-running-using-override") {
        {
            TestThread t(false);
            t.task_exec_time_ms = 1;
            CHECK(t.is_paused() == false);
            t.start();
            CHECK(t.is_paused() == false);

            CHECK(t.wait_for_task_to_run() == true);

            t.test_thread_should_exit = true;
            // This may depend on timing: the thread may or may not
            // runloop again.  Wait here for final run or timeout
            t.wait_for_task_to_run();
            task_executed = false;

            // allow task time
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            CHECK(task_executed == false);
        }
        CHECK(dtor_ran == true);
        CHECK(task_executed == false);
    }
    SUBCASE("exit-after-pause-using-override-and-dtor") {
        {
            TestThread t(false);
            t.task_exec_time_ms = 1;
            CHECK(t.is_paused() == false);
            t.start();
            CHECK(t.is_paused() == false);
            CHECK(t.wait_for_task_to_run() == true);

            t.suspend();
            CHECK(t.is_paused() == true);
            // the task should have executed
            CHECK(task_executed == true);
            task_executed = false;
            t.test_thread_should_exit = true;

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            CHECK(task_executed == false);
            // one more time as proof
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            CHECK(task_executed == false);
        }
        CHECK(dtor_ran == true);
        CHECK(task_executed == false);
    }
}