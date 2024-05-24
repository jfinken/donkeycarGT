#pragma once

#include <memory>
#include <vector>
#include "memory.hpp"
#include "parts/part.hpp"

namespace donkeycar {

/**
 * @brief Class Vehicle defines the interface to accept Parts and defines
 * the code that runs the car. Vehicle implements a loop operating at a
 * given frequency (TBD config), and it is in this loop that Parts will get
 * updated.
 * https://docs.donkeycar.com/parts/about/
 *
 * Marked final: do not derive from Vehicle
 */
class Vehicle final {
    typedef std::shared_ptr<donkeycar::Part> PartPtr;
    typedef std::shared_ptr<donkeycar::PartIO> PartDataPtr;

   public:
    Vehicle();
    ~Vehicle();

    /**
     * @brief Polymorphically add the Part
     *
     * @param part
     */
    void add(PartPtr& part);

    /**
     * @brief Start the vehicle processing loop. This is a blocking call,
     * currently SIGINT is registered
     */
    void start_blocking();

    /**
     * @brief Shutdown all parts
     */
    void stop();

    // TODO: this class itself could be manage-threaded?
    // void start_threaded();
    // void pause();
    // void run();

   private:
    // Note this works semantically as there should only ever be
    // one "vehicle" object on your car (per process).
    inline static Vehicle* cls;
    static void signal_handler(int s);

    // Loop over all parts and call their run methods, reading and writing I/O
    // data from and to the Memory store
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