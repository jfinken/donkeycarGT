#include "parts/image-list-camera.h"
#include <experimental/filesystem>
#include <iostream>

#include <opencv2/imgcodecs.hpp>

// experimental in c++14, first-class citizen in c++17
namespace fs = std::experimental::filesystem;
namespace donkeycar {

template <typename ImageT>
ImageListCamera<ImageT>::ImageListCamera(const std::string& image_path,
                                         const std::string& part_name,
                                         const std::string& in_topic,
                                         const std::string& out_topic,
                                         const bool threaded)
    : Camera<ImageT>(part_name, in_topic, out_topic, threaded) {
    auto path = fs::canonical(image_path);
    std::vector<std::string> unsorted_files;
    std::string base_path = "";

    // TODO: move this stuff to some sort of tub-utils.hpp, unit-testable
    for (const auto& entry : fs::directory_iterator(path)) {
        const auto base_name = entry.path().filename().string();
        base_path = entry.path().parent_path().string();
        constexpr auto match = ".jpg";
        constexpr auto match_size = strlen(match);
        if (base_name.size() >= match_size &&
            // Match the file by exact extension
            std::equal(base_name.end() - match_size, base_name.end(), match)) {
            unsorted_files.emplace_back(base_name);
        }
    }
    // sort by the numeric filename prefix
    std::sort(unsorted_files.begin(), unsorted_files.end(),
              [](std::string a, std::string b) {
                  int token_a = std::stoi(a.substr(0, a.find("_")));
                  int token_b = std::stoi(b.substr(0, b.find("_")));
                  return token_a < token_b;
              });
    for (const auto& f : unsorted_files) {
        // FIXME: OS separator if windows or cygwin
        m_image_filenames.emplace_back(base_path + "/" + f);
    }
    m_num_images = m_image_filenames.size();
}

template <typename ImageT>
void ImageListCamera<ImageT>::update() {
    // for now throttle it as a quick test
    int exec_time_ms = 33;
    std::this_thread::sleep_for(std::chrono::milliseconds(exec_time_ms));

    // actually get the file frame from the vector
    if (m_num_images > 0) {
        m_frame_i = (m_frame_i + 1) % m_num_images;
        // cv::Mat img = cv::imread(m_image_filenames[m_frame_i],
        // cv::IMREAD_COLOR);
        Camera<ImageT>::m_output->frame() =
            cv::imread(m_image_filenames[m_frame_i], cv::IMREAD_COLOR);
    }
}
// Explicit template instantiation
template class ImageListCamera<donkeycar::Image>;

}  // namespace donkeycar