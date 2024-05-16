
#include <mutex>
#include <future>
#include <thread>
#include <iostream>

#include "doctest/doctest.h"
#include "managed-task-status.h"

namespace concurrency = donkeycar::concurrency;

TEST_CASE("basic ManagedTaskStatus test: start_running")
{
    concurrency::ManagedTaskStatus mts("basic-mts", true);
    CHECK(mts.is_paused() == false);
    CHECK(mts.is_running() == true);
    CHECK(mts.request_pause() == false);
}

TEST_CASE("basic ManagedTaskStatus test: start_paused")
{
    concurrency::ManagedTaskStatus mts("basic-mts", false);
    CHECK(mts.is_paused() == true);
    CHECK(mts.is_running() == false);
    CHECK(mts.request_pause() == false);
}

TEST_CASE("ManagedTaskStatus test: pause_resume_logic")
{
    concurrency::ManagedTaskStatus mts("mts", false);
    REQUIRE(mts.is_paused() == true);
    bool done = false;

    // describes basic management and supervision of a thread via the MTS
    std::thread t([&done, &mts](){
        // this will block when paused
        mts.wait_to_run();
        done = true;
    });

    // thread will block until unpaused
    auto future = std::async(std::launch::async, &std::thread::join, &t);
    auto retval = future.wait_for(std::chrono::seconds(1));
    CHECK((retval == std::future_status::timeout) == true);
    CHECK(done == false);

    // kick it out of the blocked call and then we can stop it
    mts.resume_task_blocking();
    retval = future.wait_for(std::chrono::seconds(1));

    // we're done!
    CHECK((retval != std::future_status::timeout));
    CHECK(done == true);

    CHECK(t.joinable() == false);
}

TEST_CASE("ManagedTaskStatus test: exit_unblocks_wait_to_run")
{
    // start in a paused state
    concurrency::ManagedTaskStatus mts("mts", false);
    REQUIRE(mts.is_running() == false);
    bool done = false;

    std::thread t([&done, &mts](){
        // this will block until told to exit or paused
        while(mts.wait_to_run());
        done = true;
    });

    // thread will block until kicked to finished
    auto future = std::async(std::launch::async, &std::thread::join, &t);
    auto retval = future.wait_for(std::chrono::seconds(1));
    CHECK((retval == std::future_status::timeout) == true);
    CHECK(done == false);

    // kick it out of the blocked call and then we can stop it
    mts.set_exit_flag_blocking();
    retval = future.wait_for(std::chrono::seconds(1));
    
    // we're done!
    CHECK((retval != std::future_status::timeout));
    CHECK(done == true);

    CHECK(t.joinable() == false);
}
