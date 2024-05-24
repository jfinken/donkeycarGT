#include "parts/line-follower.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace donkeycar::vision {

LineFollower::LineFollower(const std::string& part_name,
                           const std::string& in_topic,
                           const std::string& out_topic, const bool threaded)
    : Part(part_name, in_topic, out_topic, threaded) {
    m_input = std::make_shared<donkeycar::Image>();
}
PartData LineFollower::run(const PartData input) {
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
void LineFollower::get_i_color(cv::Mat& src_img) {
    // grab a horizontal slice of the image via cv Mat copy ctor
    cv::Mat scan_line = src_img(cv::Rect(0, SCAN_Y, src_img.cols, SCAN_HEIGHT));

    // convert to HSV
    cv::cvtColor(scan_line, scan_line, cv::COLOR_BGR2HSV);

    // generate a mask of the colors in our desired range
    cv::Mat mask = cv::Mat::zeros(scan_line.size(), CV_8U);
    cv::Mat hist;
    cv::inRange(scan_line, COLOR_THRESHOLD_LOW, COLOR_THRESHOLD_HIGH, mask);

    // make a histogram of highest amount of yellow
    cv::reduce(mask, hist, 0, CV_REDUCE_SUM, CV_32SC1);

    // TODO output: index of max color, value of cumulative color at that index,
    // and mask of pixels in range
    // cv::Mat max_color = cv::Mat::zeros(scan_line.size(), CV_8U);
    // cv::reduceArgMax(hist, max_color, 0);  // requires opencv 4x

    diagnostic_display(mask, src_img);
}

void LineFollower::update() {
    if (m_input->frame().empty()) {
        // TODO: logger
        std::cout << "[LineFollower] Could not read the image and/or is empty"
                  << std::endl;
        return;
    }

    // copy to manipulate
    cv::Mat img = m_input->frame();
    get_i_color(img);
}

// Build a diagnostic image displaying the mask and max histogram metrics
void LineFollower::diagnostic_display(cv::Mat& mask, cv::Mat& img) {
    cv::Rect roi = cv::Rect(0, SCAN_Y, mask.cols, mask.rows);

    // colorspaces must be equivalent
    cv::cvtColor(mask, mask, cv::COLOR_GRAY2BGR);

    // Blend the ROI of A with B into the ROI of out_image
    double alpha = 0.15;
    cv::addWeighted(img(roi), alpha, mask, 1 - alpha, 0.0, img(roi));

    // scale up a bit
    cv::resize(img, img, cv::Size(img.cols * 3, img.rows * 3));

    cv::imshow("Donkeycar GT diagnotic image", img);
    cv::waitKey(2);  // 0 to wait for a keystroke
}
}  // namespace donkeycar::vision