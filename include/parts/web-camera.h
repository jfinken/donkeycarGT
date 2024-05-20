#pragma once

#include <chrono>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "parts/camera.h"

namespace donkeycar {

// A generic web cam powered by opencv

class WebCamera : public Camera {
   public:
    WebCamera(const int camera_id, const std::string& part_name,
              const std::string& in_topic, const std::string& out_topic,
              const bool threaded = false);

   private:
    virtual void update() override;

    cv::VideoCapture m_cap;
    cv::Mat m_frame;
    double m_fps;
    std::chrono::steady_clock::time_point m_last_frame;
};
}  // namespace donkeycar