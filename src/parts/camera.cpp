#include "parts/camera.h"
#include "parts/image.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace donkeycar {

Camera::Camera(const std::string& part_name, const std::string& in_topic,
               const std::string& out_topic, const bool threaded)
               : donkeycar::Part(part_name, in_topic, out_topic, threaded)
{
    m_output = std::make_shared<donkeycar::Image>();
    m_input = std::make_shared<donkeycar::Image>();
}

PartData Camera::run(const PartData input) {
    
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

void Camera::update()
{
    // quick test
    int exec_time_ms = 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(exec_time_ms));

    // actually get the frame from the camera API
    //m_output = actual_camera.capture()
    // ... modest processing, rgb-2-gray, etc
    // m_output = rgb2gray(m_output);
}

} // namespace donkeycar