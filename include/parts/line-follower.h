#pragma once

#include "parts/image.h"
#include "parts/part-io.h"
#include "parts/part.hpp"

// TODO:
// typedef std::shared_ptr<donkeycar::SteerThrottle> OutputData;
typedef std::shared_ptr<donkeycar::Image> ImageData;

namespace donkeycar::vision {
/**
 * @brief OpenCV-based controller that takes an Image as input input, generates
 * a horizontal slice of the image at a set Y coordinate. It then converts it to
 * HSV and does a color threshold to find the desired colored pixels. The
 * controller then computes a histogram to find the pixel of maximum yellow to
 * guid a PID controller which seeks to maintain the max yellow at the same
 * point in the image.
 */
class LineFollower : public donkeycar::Part {
   public:
    /**
     * @brief Construct a new LineFollower object
     *
     * @param part_name [in] assigned to the underlying thread, useful for
     * logging
     * @param in_topic [in] key used to write data within the in-memory store
     * @param out_topic [in] key used to read data within the in-memory store
     * @param threaded [in] whether or not this Part executes on a thread
     */
    LineFollower(const std::string& part_name, const std::string& in_topic,
                 const std::string& out_topic, const bool threaded = false);
    /**
     * @brief Run method overridden by this Part
     *
     * @param input data processed by this Part
     * @return PartData
     */
    virtual PartData run(const PartData input = nullptr) override;

   protected:
    ImageData m_input;

    // For now output data is generic
    // OutputData m_output;
    PartData m_output;

   private:
    /**
     * @brief Core work method for this Part. This method is a WIP
     */
    virtual void update() override;

    /**
     * @brief Get the horizontal index of the desired color from the input image
     * This method is a WIP
     * @param src_img
     */
    //
    void get_i_color(cv::Mat& src_img);

    /**
     * @brief Build a diagnostic image displaying the mask and max histogram
     * metrics
     * @param mask [in] 8-bit mask image of sliced size
     * @param img [in] source image image as input to run()
     */
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