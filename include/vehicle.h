#pragma once

// Vehicle defines the interface to accept Parts and defines the code that runs
// the car.  Very similar to the python implementation, Vehicle implements a
// loop operating at a given frequency (TBD config), and it is in this loop that
// Parts will get updated.
//
// https://docs.donkeycar.com/parts/about/

#include <memory>
#include <vector>
#include "memory.hpp"
#include "parts/part.hpp"

namespace donkeycar {

// do not really derive from Vehicle
class Vehicle final {
    typedef std::shared_ptr<donkeycar::Part> PartPtr;
    typedef std::shared_ptr<donkeycar::PartIO> PartDataPtr;

   public:
    Vehicle();
    ~Vehicle();

    void add(PartPtr& part);
    void start_blocking();
    void stop();

    // TODO: this class itself could be manage-threaded
    // void start_threaded();
    // void pause();
    // void run();

   private:
    // Note this works semantically as there should only ever be
    // one "vehicle" object on your car (per process).
    inline static Vehicle* cls;
    static void signal_handler(int s);

    void update_parts();

    // Possibly sleep based on the desired frame rate
    void throttle(std::chrono::time_point<std::chrono::steady_clock>& then);

    // Basic data structure to store parts
    std::vector<PartPtr> m_parts;

    // In-memory, thread-safe datastore for Part I/O
    std::unique_ptr<Memory> m_store;

    bool m_running{false};
    double m_fps{30.0};  // TODO: config system
};
}  // namespace donkeycar