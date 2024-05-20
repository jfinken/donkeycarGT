#pragma once

#include <opencv2/imgproc.hpp>
#include "parts/part-io.h"

namespace donkeycar {
class Image : public PartIO {
   public:
    Image();
    cv::Mat& frame();

   protected:
    cv::Mat m_frame;
};
}  // namespace donkeycar