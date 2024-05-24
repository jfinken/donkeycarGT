#pragma once

#include "managed-thread.h"
#include "part-io.h"

typedef std::shared_ptr<donkeycar::PartIO> PartData;

namespace donkeycar {

/**
 * @brief Part is not only an ABC that defines an interface invoked via Vehicle,
 * but it is also a ManagedThread thus completely hiding concurrency semantics.
 */
class Part : public donkeycar::concurrency::ManagedThread {
   public:
    /**
     * @brief Construct a new Part object.  Note: if threaded, start paused
     * @param part_name
     * @param in_topic
     * @param out_topic
     * @param threaded
     */
    Part(const std::string part_name, const std::string& in_topic,
         const std::string& out_topic, const bool threaded)
        : donkeycar::concurrency::ManagedThread(part_name, true),
          m_input_topic(in_topic),
          m_output_topic(out_topic),
          m_threaded(threaded) {
        m_part_name = part_name;
        // m_output = std::make_shared<PartIO>();
        // m_input = std::make_shared<PartIO>();
    }

    virtual ~Part() {}

    /**
     * @brief If this Part is threaded, create and start the thread under
     * management
     */
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

    /**
     * @brief If this Part is threaded, exit the thread's work loop
     * and join.
     */
    virtual void shutdown() {
        if (m_threaded) {
            donkeycar::concurrency::ManagedThread::graceful_exit();
            donkeycar::concurrency::ManagedThread::join();
        }
    }

    /**
     * @brief Return this Part's output topic.  This is used a key into the
     * Vehicle's memory store.
     *
     * @return const std::string&
     */
    const std::string& output_topic() { return m_output_topic; }

    /**
     * @brief Return this Part's input topic.  This is used a key into the
     * Vehicle's memory store.
     *
     * @return const std::string&
     */
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