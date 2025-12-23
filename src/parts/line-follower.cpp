#include "parts/line-follower.h"
#include <iomanip>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>

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

// Returns: centerline position, total pixels in line, and the mask
std::tuple<int, int, cv::Mat> LineFollower::get_i_color(cv::Mat& src_img) {
    // grab a horizontal slice of the image via cv Mat copy ctor
    cv::Mat scan_line = src_img(cv::Rect(0, SCAN_Y, src_img.cols, SCAN_HEIGHT));

    // convert to HSV
    cv::Mat hsv;
    cv::cvtColor(scan_line, hsv, cv::COLOR_BGR2HSV);

    // generate a mask of the colors in our desired range
    cv::Mat mask;
    cv::inRange(hsv, COLOR_THRESHOLD_LOW, COLOR_THRESHOLD_HIGH, mask);

    // Optional: Apply morphological operations to reduce noise
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);  // Close small gaps
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);  // Remove small noise

    // Calculate column-wise sum (histogram of vertical pixel counts)
    cv::Mat hist;
    cv::reduce(mask, hist, 0, cv::REDUCE_SUM, CV_32SC1);

    // Find the column with maximum white pixels (the line center)
    double min_val, max_val;
    cv::Point min_loc, max_loc;
    cv::minMaxLoc(hist, &min_val, &max_val, &min_loc, &max_loc);

    int centerline_x = max_loc.x;
    int total_pixels =
        static_cast<int>(max_val / 255);  // Convert sum to pixel count

    return std::make_tuple(centerline_x, total_pixels, mask);
}

void LineFollower::update() {
    if (m_input->frame().empty()) {
        // TODO: logger
        std::cout << "[LineFollower] Could not read the image and/or is empty"
                  << std::endl;
        return;
    }

    // copy to manipulate
    cv::Mat img = m_input->frame().clone();
    auto [centerline_x, total_pixels, mask] = get_i_color(img);
    // Store results for use by other parts (steering controller, etc.)
    // TODO: Update m_output with centerline_x and total_pixels

    std::cout << "[LineFollower] Line center at x=" << centerline_x
              << ", pixels=" << total_pixels << std::endl;

    diagnostic_display(mask, img, centerline_x, total_pixels);
}

// Build a diagnostic image displaying the mask and line detection metrics
void LineFollower::diagnostic_display(cv::Mat& mask, cv::Mat& img,
                                      int centerline_x, int total_pixels) {
    cv::Rect roi = cv::Rect(0, SCAN_Y, mask.cols, mask.rows);

    // Create a colored overlay for the mask
    cv::Mat mask_colored;
    cv::cvtColor(mask, mask_colored, cv::COLOR_GRAY2BGR);

    // Make the mask green for better visibility
    cv::Mat mask_green =
        cv::Mat::zeros(mask_colored.size(), mask_colored.type());
    mask_green.setTo(cv::Scalar(0, 255, 0), mask);  // Green where mask is white

    // Blend the ROI with the green mask
    double alpha = 0.3;
    cv::addWeighted(img(roi), 1.0, mask_green, alpha, 0.0, img(roi));

    // Draw the scan region boundaries
    cv::rectangle(img, cv::Point(0, SCAN_Y),
                  cv::Point(img.cols - 1, SCAN_Y + SCAN_HEIGHT - 1),
                  cv::Scalar(255, 255, 0), 1);  // Yellow rectangle

    // Draw the detected centerline (vertical line at the line center)
    // Only draw if we detected enough pixels
    if (total_pixels >= MIN_DETECTION_THRESHOLD) {
        int line_top = SCAN_Y;
        int line_bottom = SCAN_Y + SCAN_HEIGHT;

        // Draw the centerline in red
        cv::line(img, cv::Point(centerline_x, line_top),
                 cv::Point(centerline_x, line_bottom), cv::Scalar(0, 0, 255),
                 1);  // Red line, thickness 3

        // Draw a filled circle at the centerline position
        cv::circle(img, cv::Point(centerline_x, SCAN_Y + SCAN_HEIGHT / 2), 4,
                   cv::Scalar(0, 0, 255), -1);
    }

    // Draw the target centerline (where we want the line to be, typically image
    // center)
    int target_x = img.cols / 2;
    cv::line(img, cv::Point(target_x, SCAN_Y),
             cv::Point(target_x, SCAN_Y + SCAN_HEIGHT), cv::Scalar(255, 0, 255),
             1);  // Magenta line for target

    // Add text information with fixed-width, col alignment
    std::ostringstream oss;
    oss << std::left << std::setw(8) << "Center:" << std::right << std::setw(4)
        << centerline_x << " " << std::left << std::setw(8)
        << "Pixels:" << std::right << std::setw(4) << total_pixels;
    cv::putText(img, oss.str(), cv::Point(5, 10), cv::FONT_HERSHEY_SIMPLEX, 0.3,
                cv::Scalar(255, 0, 0), 1);

    // Calculate error from center with column alignment
    int error = centerline_x - target_x;
    oss.str("");  // Clear the stringstream
    oss.clear();  // Clear any error flags
    oss << std::left << std::setw(8) << "Error:" << std::right << std::setw(4)
        << error << std::right << std::setw(3) << " px";
    cv::Scalar error_color =
        (error < 0) ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 0, 255);
    cv::putText(img, oss.str(), cv::Point(5, 20), cv::FONT_HERSHEY_SIMPLEX, 0.3,
                error_color, 1);

    // scale up for better visibility and to match the scale of the
    // image-consumer
    cv::resize(img, img, cv::Size(img.cols * 3, img.rows * 3));

    cv::imshow("DonkeycarGT diagnostic image", img);
    cv::waitKey(2);  // 2ms wait
}

}  // namespace donkeycar::vision