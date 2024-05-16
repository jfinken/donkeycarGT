
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "parts/image-consumer.h"
#include "parts/image.h"

namespace donkeycar {

ImageConsumer::ImageConsumer(
    const std::string& part_name,
    const std::string& in_topic,
    const std::string& out_topic, 
    const bool threaded)
    : Part(part_name, in_topic, out_topic, threaded)
{
    m_input = std::make_shared<donkeycar::Image>();
}

PartData ImageConsumer::run(const PartData input) {

    m_input = std::static_pointer_cast<donkeycar::Image>(input);

    if(this->m_threaded) {
        // just get latest cached
        return m_output;
    } else {
        // directly call work method (also called by thread if threaded)
        update();
        return m_output;
    }
}

void ImageConsumer::update()
{
    if(m_input->frame().empty())
    {
        // TODO: logger
        std::cout << "[ImageConsumer] Could not read the image" << std::endl;
        return;
    }
    
    int rows = m_input->frame().rows;
    int cols = m_input->frame().cols;
    // copy to manipulate 
    cv::Mat img = m_input->frame();
    cv::resize(img, img, cv::Size(cols * 3, rows * 3));

    // cv::imshow("Display window", m_input->frame());
    cv::imshow("Donkeycar GT", img);
    cv::waitKey(2); // Wait for a keystroke in the window

}
} // namespace donkeycar