#pragma once

#include "parts/image.h"
#include "parts/part-io.h"
#include "parts/part.hpp"

// OpenCV-based controller that takes as input a horizontal slice of the image
// at a set Y coordinate. It then converts it to HSV and does a color thresh
// hold to find the yellow pixels. The controller then computes a histogram to
// find the pixel of maximum yellow to guid a PID controller which seeks to
// maintain the max yellow at the same point in the image.

// TODO:
// typedef std::shared_ptr<donkeycar::SteerThrottle> OutputData;
typedef std::shared_ptr<donkeycar::Image> ImageData;

namespace donkeycar::vision {
class LineFollower : public donkeycar::Part {
   public:
    LineFollower(const std::string& part_name, const std::string& in_topic,
                 const std::string& out_topic, const bool threaded = false);
    virtual PartData run(const PartData input = nullptr) override;

   protected:
    ImageData m_input;

    // For now output data is generic
    // OutputData m_output;
    PartData m_output;

   private:
    virtual void update() override;

    // Get the horizontal index of the color at the given image slice
    void get_i_color(cv::Mat& src_img);

    // Build a diagnostic image displaying the mask and max histogram metrics
    void diagnostic_display(cv::Mat& mask, cv::Mat& img);

    // TODO:
    //  - implement a config file solution, e.g. JSON, YAML
    //  - read in the config for the below
    // num pixels from the top to start horiz scan
    const int SCAN_Y = 100;
    // num pixels high to grab from horiz scan
    const int SCAN_HEIGHT = 20;
    // HSV dark yellow (opencv HSV hue value is 0..179, saturation and value are
    // both 0..255)
    const cv::Scalar COLOR_THRESHOLD_LOW = cv::Scalar(0, 80, 80);
    // HSV light yellow (opencv HSV hue value is 0..179, saturation and value
    // are both 0..255)
    const cv::Scalar COLOR_THRESHOLD_HIGH = cv::Scalar(80, 255, 255);
};
}  // namespace donkeycar::vision