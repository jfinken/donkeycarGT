#pragma once

#include "part-io.h"
#include "parts/image.h"
#include "parts/part.hpp"

// TODO: make this a class template if/when need differing image types here
// TODO: standardize the location of these typedefs?
typedef std::shared_ptr<donkeycar::Image> ImageData;

namespace donkeycar {

/**
 * @brief Class for consuming donkeycar::Images.
 *
 */
class ImageConsumer : public Part {
   public:
    /**
     * @brief Construct a new ImageConsumer object
     *
     * @param part_name [in] assigned to the underlying thread, useful for
     * logging
     * @param in_topic [in] key used to write data within the in-memory store
     * @param out_topic [in] key used to read data within the in-memory store
     * @param threaded [in] whether or not this Part executes on a thread
     */
    ImageConsumer(const std::string& part_name, const std::string& in_topic,
                  const std::string& out_topic, const bool threaded = false);
    /**
     * @brief Run method overridden by this ImageConsumer Part
     *
     * @param input data processed by this Part
     * @return PartData
     */
    virtual PartData run(const PartData input = nullptr) override;

   protected:
    ImageData m_input;
    ImageData m_output;

   private:
    /**
     * @brief Core work method for Parts. ImageConsumer will receive
     * a frame and display it.
     */
    virtual void update() override;
};
}  // namespace donkeycar