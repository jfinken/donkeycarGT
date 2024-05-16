#pragma once

#include "part-io.h"
#include "managed-thread.h"

namespace donkeycar {

typedef std::shared_ptr<donkeycar::PartIO> PartData;

// Part defines an interface invoked via Vehicle.  TODO: more doc
class Part : public donkeycar::concurrency::ManagedThread
{
public:
    Part(const std::string part_name, const std::string& in_topic,
         const std::string& out_topic, const bool threaded = false);
    virtual ~Part();
    virtual void start() override;
    virtual void shutdown();
    // Note: no run_threaded API.  If threaded, then the Part will
    // have already called update.
    virtual PartData run(const PartData input = nullptr) = 0;
    virtual void update() = 0;

    // API for Part input and outputs
    //virtual PartData get_input_ptr() = 0; 
    //virtual PartData get_output_ptr() = 0; 

    const std::string& output_topic();
    const std::string& input_topic();

protected:
    //PartData m_output;
    //PartData m_input;
    std::string m_input_topic;
    std::string m_output_topic;
    std::string m_part_name;
    bool m_threaded;
};
} // namespace donkeycar