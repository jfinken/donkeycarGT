#include <iostream>
#include <experimental/filesystem>
#include "parts/image-list-camera.h"

//c++14
namespace fs = std::experimental::filesystem;
namespace donkeycar {

ImageListCamera::ImageListCamera(const std::string& image_path,
    const std::string& part_name,
    const std::string& in_topic,
    const std::string& out_topic, 
    const bool threaded)
    : Camera(part_name, in_topic, out_topic, threaded)
{
    for (const auto & entry : fs::directory_iterator(image_path)) {
        
        const auto base_name = entry.path().filename().string();
        constexpr auto match = ".jpg";
        constexpr auto match_size = strlen(match);
        if(base_name.size() >= match_size &&
            // Match the file by exact extension
	        std::equal(base_name.end() - match_size, base_name.end(), match)) {
                m_image_filenames.emplace_back(entry.path().string());
        }
    }
    // TODO: custom sort lamba: split by _ then take 0th
    std::sort(m_image_filenames.begin(), m_image_filenames.end());
    for(const auto& f : m_image_filenames) {
        std::cout << f << std::endl;
    }
    m_num_images = m_image_filenames.size();
}

void ImageListCamera::update()
{
    // quick test
    std::cout << "ImageListCamera::update..." << std::endl;
    int exec_time_ms = 33;
    std::this_thread::sleep_for(std::chrono::milliseconds(exec_time_ms));

    // actually get the file frame from the vector 
    //m_output = cv.load(filename) 

    int rows = m_output->frame().rows;
    int cols = m_output->frame().cols;
    printf("ImageListCamera::update resulting image: %d x %d\n", cols, rows);
}
} // namespace donkeycar