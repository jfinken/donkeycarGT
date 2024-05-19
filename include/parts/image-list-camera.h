#pragma once

#include "parts/camera.hpp"

namespace donkeycar {

template<typename ImageT>
class ImageListCamera : public Camera<ImageT>
{
public:
    ImageListCamera(const std::string& image_path,
                    const std::string& part_name, const std::string& in_topic,
                    const std::string& out_topic, const bool threaded = false);

private:
    virtual void update() override;

    std::vector<std::string> m_image_filenames;
    int m_num_images{0};
    int m_frame_i{0};
};
} // namespace donkeycar