#include "vehicle.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace donkeycar {

Vehicle::Vehicle() {
    m_store = std::make_unique<Memory>();

    cls = this;
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = Vehicle::signal_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}
Vehicle::~Vehicle() {}
void Vehicle::add(PartPtr& part) { m_parts.emplace_back(part); }

// TODO: register keyboard sigint to call stop
void Vehicle::start_blocking() {
    m_running = true;
    for (const PartPtr& part : m_parts) {
        part->start();
    }
    while (m_running) {
        update_parts();
        //  throttle the run loop via a max_loop_count or sleepy-time per
        //  desired frequency
    }
}

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
 *   3. A Part takes an input but produces no output (e.g. mqtt image publisher)
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