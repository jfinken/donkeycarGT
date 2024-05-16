
#include <mutex>
#include <future>
#include <thread>
#include <chrono>
#include <iostream>
#include <cstdlib>

#include "doctest/doctest.h"
#include "event.h"
#include "lock.hpp"
#include "mutex.h"

namespace concurrency = donkeycar::concurrency;

int get_random(int lower, int higher) {
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    return lower + static_cast<int>((static_cast<float>(higher-lower)) * r);
}

TEST_CASE("basic event test")
{
    concurrency::Mutex mutex("test-mutex");
    concurrency::Event event;
    
    std::thread t([&mutex, &event]() {

        concurrency::Lock lock(mutex);
        std::cerr << "[test-event] waiter thread waiting..." << std::endl;
        event.wait(mutex);
        std::cerr << "[test-event] waiter thread signaled via event." << std::endl;
        std::cerr << "[test-event] waiter thread done." << std::endl;
    });

    // thread will block until unpaused
    auto future = std::async(std::launch::async, &std::thread::join, &t);
    auto retval = future.wait_for(std::chrono::seconds(1));
    CHECK((retval == std::future_status::timeout) == true);

    // sleep this main thread
    int sleep = get_random(1000, 4000);
    {
        std::cerr << "[test-event] main thread sleeping for " << sleep << " ms ..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        {
            concurrency::Lock lock(mutex);
            std::cerr << "[test-event] main thread triggering all via event." << std::endl;
        }
        // release t
        event.trigger_all();
    }
    // and we're done
    retval = future.wait_for(std::chrono::milliseconds(500));
    CHECK((retval != std::future_status::timeout));
    CHECK(t.joinable() == false);
}
