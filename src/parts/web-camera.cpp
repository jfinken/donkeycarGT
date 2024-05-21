#include "parts/web-camera.h"
#include <iostream>

namespace donkeycar {

template <typename ImageT>
WebCamera<ImageT>::WebCamera(const int camera_id, const std::string& part_name,
                             const std::string& in_topic,
                             const std::string& out_topic, const bool threaded)
    : Camera<ImageT>(part_name, in_topic, out_topic, threaded) {
    // "Corrupt JPEG data: 2 extraneous bytes before marker" warnings
    // https://github.com/opencv/opencv/issues/9477
    // Likely fixed in newer opencv
    m_cap.open(camera_id);

    // TODO: camera and image params per a config
    m_fps = 30.0;
    m_period_ms = (1.0 / m_fps * 1000);
    // m_cap.set(cv::CAP_PROP_FRAME_WIDTH, image_width_);
    // m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, image_height_);
}

template <typename ImageT>
void WebCamera<ImageT>::update() {
    m_cap >> m_frame;

    // throttle via the minimum desired frequency
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - m_last_frame)
                        .count();
    // Note if period_ms have not elapsed since the last update
    // downstream consumers will consume the previous (valid) m_frame
    if (!m_frame.empty() && (duration > m_period_ms)) {
        m_last_frame = now;
        Camera<ImageT>::m_output->frame() = m_frame;
    }

}  // namespace donkeycar
// Explicit template instantiation
template class WebCamera<donkeycar::Image>;

}  // namespace donkeycar