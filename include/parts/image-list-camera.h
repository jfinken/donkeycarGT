#pragma once

#include "parts/camera.hpp"

// A "camera" that loads image files from disk

namespace donkeycar {

/**
 * @brief Class representing a concrete Camera to load Tub image files from
 * disk.
 */
template <typename ImageT>
class ImageListCamera : public Camera<ImageT> {
   public:
    /**
     * @brief Construct a new ImageListCamera object
     *
     * @param image_path [in] fully-qualified path to a directory contain Tub
     * imagery
     * @param part_name [in] assigned to the underlying thread, useful for
     * logging
     * @param in_topic [in] key used to write data within the in-memory store
     * @param out_topic [in] key used to read data within the in-memory store
     * @param threaded [in] whether or not this Part executes on a thread
     */
    ImageListCamera(const std::string& image_path, const std::string& part_name,
                    const std::string& in_topic, const std::string& out_topic,
                    const bool threaded = false);

   private:
    /**
     * @brief Core work method for Parts. ImageListCamera will load image files
     * from disk to a list, sort, then iterate the list return PartData objects.
     */
    virtual void update() override;

    std::vector<std::string> m_image_filenames;
    int m_num_images{0};
    int m_frame_i{0};
};
}  // namespace donkeycar