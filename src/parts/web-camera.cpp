#include <iostream>
#include "parts/web-camera.h"

namespace donkeycar {

WebCamera::WebCamera(const int camera_id,
    const std::string& part_name,
    const std::string& in_topic,
    const std::string& out_topic, 
    const bool threaded)
    : Camera(part_name, in_topic, out_topic, threaded)
{
    // "Corrupt JPEG data: 2 extraneous bytes before marker"
    // https://github.com/opencv/opencv/issues/9477
    // Likely fixed in newer opencv
    m_cap.open(camera_id);

    // TODO: camera and image params per a config
    m_fps = 33.0;
    //m_cap.set(cv::CAP_PROP_FRAME_WIDTH, image_width_);
    //m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, image_height_);
}

void WebCamera::update()
{
    m_cap >> m_frame;

    // throttle via the minimum desired frequency 
    auto now = std::chrono::steady_clock::now();
    //double fps = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_frame_).count();
    if (!m_frame.empty() &&
        std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_frame).count() > 1/m_fps*1000)
    {
        m_last_frame = now;
        m_output->frame() = m_frame;
    }
}
} // namespace donkeycar