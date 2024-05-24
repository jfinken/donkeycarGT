#pragma once

#include <opencv2/imgproc.hpp>
#include "parts/part-io.h"

namespace donkeycar {
/**
 * @brief Class representing a logical Image. Deriving from PartIO, Image
 * objects can be read and written from and to the Vehicle memory store, and
 * thus be delivered and received to and from Parts.
 */
class Image : public PartIO {
   public:
    /**
     * @brief Construct a new Image object
     */
    Image();
    /**
     * @brief Access the underlying data structure, a cv::Mat in this case
     * @return cv::Mat&
     */
    cv::Mat& frame();

   protected:
    cv::Mat m_frame;
};
}  // namespace donkeycar