
#include "parts/image.h"

namespace donkeycar {

Image::Image() : PartIO()
{
}

cv::Mat& Image::frame()
{
    return m_frame;
}
} // namespace donkeycar