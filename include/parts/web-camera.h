#pragma once

#include <chrono>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "parts/camera.hpp"

// A generic web cam powered by opencv

namespace donkeycar {

/**
 * @brief  WebCamera is a class representing a generic web camera powered by
 * OpenCV.
 */
template <typename ImageT>
class WebCamera : public Camera<ImageT> {
   public:
    /**
     * @brief Construct a new WebCamera object
     *
     * @param camera_id [in] device-descriptor number identifying the desired
     * camera. For example, if /dev/video0 and /dev/video1, then supply 1 to
     * consume imagery from /dev/video1
     * @param part_name [in] assigned to the underlying thread, useful for
     * logging
     * @param in_topic [in] key used to write data within the in-memory store
     * @param out_topic [in] key used to read data within the in-memory store
     * @param threaded [in] whether or not this Part executes on a thread
     */
    WebCamera(const int camera_id, const std::string& part_name,
              const std::string& in_topic, const std::string& out_topic,
              const bool threaded = false);

   private:
    virtual void update() override;

    cv::VideoCapture m_cap;
    cv::Mat m_frame;
    double m_fps;
    double m_period_ms;
    std::chrono::steady_clock::time_point m_last_frame;
};
}  // namespace donkeycar