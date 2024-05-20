#pragma once

#include "part-io.h"
#include "parts/image.h"
#include "parts/part.hpp"

// TODO: make this a class template if/when need differing image types here
// TODO: standardize the location of these typedefs?
typedef std::shared_ptr<donkeycar::Image> ImageData;

namespace donkeycar {

class ImageConsumer : public Part {
   public:
    ImageConsumer(const std::string& part_name, const std::string& in_topic,
                  const std::string& out_topic, const bool threaded = false);
    virtual PartData run(const PartData input = nullptr) override;

   protected:
    ImageData m_input;
    ImageData m_output;

   private:
    virtual void update() override;
};
}  // namespace donkeycar