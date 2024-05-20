#pragma once

#include "managed-thread.h"
#include "part-io.h"

typedef std::shared_ptr<donkeycar::PartIO> PartData;

namespace donkeycar {

// Part defines an interface invoked via Vehicle.  TODO: more doc
class Part : public donkeycar::concurrency::ManagedThread {
   public:
    Part(const std::string part_name, const std::string& in_topic,
         const std::string& out_topic, const bool threaded)
        : donkeycar::concurrency::ManagedThread(part_name, true),
          m_input_topic(in_topic),
          m_output_topic(out_topic),
          m_threaded(threaded) {
        // Note: if threaded, start paused

        m_part_name = part_name;
        // m_output = std::make_shared<PartIO>();
        // m_input = std::make_shared<PartIO>();
    }

    virtual ~Part() {}

    virtual void start() override {
        // start will create the thread so only if we're threaded
        if (m_threaded) {
            // will create the thread
            donkeycar::concurrency::ManagedThread::start();
            // set it to running
            donkeycar::concurrency::ManagedThread::resume();
        }
    }

    virtual PartData run(const PartData input = nullptr) = 0;

    virtual void update() = 0;

    virtual void shutdown() {
        if (m_threaded) {
            donkeycar::concurrency::ManagedThread::graceful_exit();
            donkeycar::concurrency::ManagedThread::join();
        }
    }

    const std::string& output_topic() { return m_output_topic; }
    const std::string& input_topic() { return m_input_topic; }

   protected:
    // PartData m_output;
    // PartData m_input;

    std::string m_input_topic;
    std::string m_output_topic;
    std::string m_part_name;
    bool m_threaded;
};
}  // namespace donkeycar