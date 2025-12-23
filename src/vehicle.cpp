#include "vehicle.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <iostream>

namespace donkeycar {

Vehicle::Vehicle() {
    m_store = std::make_unique<Memory>();

    // register sigint to gracefully call stop
    cls = this;
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = Vehicle::signal_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}
Vehicle::~Vehicle() {}
void Vehicle::add(PartPtr& part) { m_parts.emplace_back(part); }

void Vehicle::start_blocking() {
    m_running = true;
    for (const PartPtr& part : m_parts) {
        part->start();
    }
    while (m_running) {
        auto then = std::chrono::steady_clock::now();

        update_parts();

        // Ability to throttle the system by a supplied loop-count or frequency
        throttle(then);
    }
}

// Calculate the delta between the frame duration and desired frame period,
// and sleep for that amount.  Note this doesn't impact threaded parts insofar
// as they will produce and consume data on their individual frequencies.
void Vehicle::throttle(
    std::chrono::time_point<std::chrono::steady_clock>& then) {
    auto now = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - then)
            .count();
    auto period_ns = (1.0 / m_fps * 1e9);
    int sleepy_time_ms = (int)(period_ns - duration) / 1e6;
    if (sleepy_time_ms > 0.0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepy_time_ms));
        // printf("[Vehicle] sleeping for %d ms (update_part duration: %.2f
        // ms)\n",
        //       sleepy_time_ms, (duration / 1e6));
    }
}

// Shutdown all parts
void Vehicle::stop() {
    m_running = false;
    for (const PartPtr& part : m_parts) {
        part->shutdown();
    }
}

/*
 * Run each part sequentially.  Note that if the Part is threaded, its
 * run method will simply return the latest cached value. If it is not
 * threaded it will invoke the core work method, update.
 *
 * Part and IO requirements
 *   1. A Part takes no input but produces an output (e.g. camera)
 *   2. A Part takes input and produces output (e.g. image-conversion)
 *   3. A Part takes an input but produces no output (e.g. mqtt publisher)
 */
void Vehicle::update_parts() {
    for (const PartPtr& part : m_parts) {
        // Run the Part with possible input
        PartDataPtr input = m_store->get(part->input_topic());
        PartDataPtr output = part->run(input);

        // Save the Part output to the memory store
        m_store->put(part->output_topic(), output);
    }
}
void Vehicle::signal_handler(int s) {
    printf("[Vehicle] Caught signal %d.  Shutting down...\n", s);
    cls->stop();
}
}  // namespace donkeycar