#pragma once

#include "parts/part.h"
#include "part-io.h"
#include "parts/image.h"

namespace donkeycar {

typedef std::shared_ptr<donkeycar::Image> ImageData;

class ImageConsumer : public Part 
{
public:
    ImageConsumer(const std::string& part_name, const std::string& in_topic,
                 const std::string& out_topic, const bool threaded = false);
    virtual PartData run(const PartData input = nullptr) override;

protected:
    // TODO: should these be data members of part.h?
    ImageData m_input;
    ImageData m_output;

private:
    virtual void update() override;
};
} // namespace donkeycar