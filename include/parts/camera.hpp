#pragma once

#include <chrono>
#include <thread>
#include "image.h"
#include "part-io.h"
#include "part.hpp"

namespace donkeycar {

/**
 * @brief  Class representing a logical Camera. Concrete cameras should decide
 * on ImageT data, derive from this class, override run() and update().
 */
template <typename ImageT>
class Camera : public donkeycar::Part {
   public:
    typedef std::shared_ptr<ImageT> ImageData;

    /**
     * @brief Construct a new Camera object
     *
     * @param part_name [in] assigned to the underlying thread, useful for
     * logging
     * @param in_topic [in] key used to write data within the in-memory store
     * @param out_topic [in] key used to read data within the in-memory store
     * @param threaded [in] whether or not this Part executes on a thread
     */
    Camera(const std::string& part_name, const std::string& in_topic,
           const std::string& out_topic, const bool threaded = false)
        : donkeycar::Part(part_name, in_topic, out_topic, threaded) {
        m_output = std::make_shared<ImageT>();
        m_input = std::make_shared<ImageT>();
    }

   protected:
    /**
     * @brief Run method overridden by this Camera and derived Camera Parts
     *
     * @param input data processed by this Part
     * @return PartData
     */
    virtual PartData run(const PartData input) {
        // -Werr=unused-parameter
        // so for now
        (void)input;

        if (this->m_threaded) {
            // just get latest cached
            return m_output;
        } else {
            // directly call work method (also called by thread if threaded)
            update();
            return m_output;
        }
    }

    /**
     * @brief Core work method for Parts.  Here more of a placeholder to be
     * overridden by concrete Cameras.
     */
    virtual void update() override {
        // quick test
        int exec_time_ms = 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(exec_time_ms));
    }

    // Crucially these are template-type specific, but are returned
    // (polymorphically) corrected with the run API
    ImageData m_output;
    ImageData m_input;
};
}  // namespace donkeycar
