#pragma once

#include <chrono>
#include "image.h"
#include "part-io.h"
#include "part.hpp"

/* // See note below
#include "imu.h"
#include "laser-scan.h"
#include "nav-sat-fix.h"
#include "pose.h"
*/

namespace donkeycar {

// The interesting bits here are the data types to be
// serialized and put over the network, and the de/serialization
// implementation options, e.g. protocol buffers (obviously
// python.pickle is out of the question :)
// typedef std::shared_ptr<donkeycar::Image> ImageData;
// typedef std::shared_ptr<donkeycar::Pose> PoseData;
// typedef std::shared_ptr<donkeycar::Imu> ImuData;

/*

// A Network publisher publishes messages of any type to a topic.

// This is what I want:
shared_ptr<donkeycar::Part<donkeycar::PartIO>> mqtt_pub =
    std::make_shared<donkeycar::NetworkPublisher<donkeycar::Pose>>
        (pub_topic, mem_in_topic, mem_out_topic, threaded=true);

// Likely:
class NetworkPublisher;
class NetworkPublisherMqtt : publish NetworkPublisher;

// Or maybe a plugin system with pluginlib
*/

template <typename MessageT>
class NetworkPublisherMqtt : public donkeycar::Part {
   public:
    typedef std::shared_ptr<MessageT> NetworkData;

    NetworkPublisherMqtt(const std::string& xfer_topic,
                         const std::string& in_topic,
                         const std::string& out_topic,
                         const bool threaded = false)
        : donkeycar::Part(xfer_topic, in_topic, out_topic, threaded) {
        m_output = std::make_shared<MessageT>();
        m_input = std::make_shared<MessageT>();
    }
    virtual PartData run(const PartData input = nullptr) override {
        m_input = std::static_pointer_cast<MessageT>(input);

        if (this->m_threaded) {
            // just get latest cached
            return m_output;
        } else {
            // directly call work method (also called by thread if threaded)
            update();
            return m_output;
        }
    }

   protected:
    virtual void update() override {
        // serialization (common code)

        // publish
    }

    // Data and interface implementation for input-output from the Part class
    NetworkData m_output;
    NetworkData m_input;
};
}  // namespace donkeycar
