#pragma once

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgcodecs.hpp>
#include <chrono>
#include "parts/camera.h"

namespace donkeycar {

// A generic web cam powered by opencv

class WebCamera : public Camera
{
public:
    WebCamera(const int camera_id,
        const std::string& part_name, const std::string& in_topic,
        const std::string& out_topic, const bool threaded = false);

private:
    virtual void update() override;

    cv::VideoCapture cap_;
    cv::Mat frame_;
    double fps_;
    std::chrono::steady_clock::time_point last_frame_;
};
} // namespace donkeycar