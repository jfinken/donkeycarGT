#pragma once

#include "parts/image.h"
#include "parts/interpreter.hpp"
#include "parts/part-io.h"
#include "parts/part.hpp"

// TODO:
// typedef std::shared_ptr<donkeycar::SteerThrottle> OutputData;
typedef std::shared_ptr<donkeycar::Image> ImageData;

namespace donkeycar::vision {

class KerasPilot : public donkeycar::Part {
   public:
    KerasPilot(const std::string& part_name, const std::string& in_topic,
               const std::string& out_topic, const bool threaded = false)
        : Part(part_name, in_topic, out_topic, threaded) {
        m_input = std::make_shared<donkeycar::Image>();
        m_interpreter =
            std::make_unique<donkeycar::vision::TfLiteInterpreter>();
        // input_shape

        // TODO: make this an env var
        std::string model_path =
            "/root/mycar/data/murmurpi4_circuit_launch_20210716_1826/models/"
            "pilot_21-08-13_19.tflite";
        m_interpreter->load(model_path);
    }
    virtual void load(const std::string model_path) {
        m_interpreter->load(model_path);
    }
    /**
     * @brief Run method overridden by this Part.  This will invoke the
     * inference method of the Interpreter
     *
     * @param input data processed by this Part
     * @return PartData
     */
    virtual PartData run(const PartData input = nullptr) override {
        m_input = std::static_pointer_cast<donkeycar::Image>(input);

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
    ImageData m_input;

    // For now output data is generic
    // OutputData m_output;
    PartData m_output;

   private:
    /**
     * @brief Core work method for this Part. This method is a WIP
     */
    virtual void update() override { m_interpreter->predict(); }

    std::unique_ptr<donkeycar::vision::Interpreter> m_interpreter;
};
}  // namespace donkeycar::vision