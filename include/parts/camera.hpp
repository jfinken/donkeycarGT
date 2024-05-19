#pragma once

#include <thread>
#include <chrono>
#include "part.hpp"
#include "part-io.h"
#include "image.h"

namespace donkeycar
{

// TODO: to allow the develop to stamp out different image-based message types?
template<typename ImageT> 
class Camera : public donkeycar::Part
{
public:
    typedef std::shared_ptr<ImageT> ImageData;

    // crucially this is needed to override the API
    // but note that it returns the type-specific data, ImageData
    typedef std::shared_ptr<donkeycar::PartIO> PartData;

    Camera(const std::string& part_name, const std::string& in_topic,
           const std::string& out_topic, const bool threaded = false)
    : donkeycar::Part(part_name, in_topic, out_topic, threaded)
    {
        m_output = std::make_shared<ImageT>();
        m_input = std::make_shared<ImageT>();
    }
protected:
    virtual PartData run(const PartData input) {
        
        // -Werr=unused-parameter
        // so for now
        (void)input;

        if(this->m_threaded) {
            // just get latest cached
            return m_output;
        } else {
            // directly call work method (also called by thread if threaded)
            update();
            return m_output;
        }
    }

    virtual void update() override
    {
        // quick test
        int exec_time_ms = 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(exec_time_ms));
    }

    // Crucially these are template-type specific, but are returned (polymorphically) corrected
    // with the run API 
    ImageData m_output;
    ImageData m_input;
    //ImageData get_input_ptr() {return m_input;}
    //ImageData get_output_ptr() {return m_output;}

};
} // namespace donkeycar
