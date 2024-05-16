#pragma once

#include "part.h"
#include "part-io.h"
#include "image.h"

namespace donkeycar
{

typedef std::shared_ptr<donkeycar::Image> ImageData;

class Camera : public donkeycar::Part
{
public:
    Camera(const std::string& part_name, const std::string& in_topic,
           const std::string& out_topic, const bool threaded = false);
    virtual PartData run(const PartData input = nullptr) override;

protected:
    virtual void update() override;

    // Data and interface implementation for input-output from the Part class
    ImageData m_output;
    ImageData m_input;
    //PartData get_input_ptr() {return m_input;}
    //PartData get_output_ptr() {return m_output;}

};
} // namespace donkeycar
