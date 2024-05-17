#include "parts/part.h"

namespace donkeycar 
{

Part::Part(const std::string part_name, const std::string& in_topic,
    const std::string& out_topic, const bool threaded) 
    : donkeycar::concurrency::ManagedThread(part_name, true)
    , m_input_topic(in_topic)
    , m_output_topic(out_topic)
    , m_threaded(threaded) 
{
    // Note: if threaded, start paused

    m_part_name = part_name;
    m_output = std::make_shared<donkeycar::PartIO>();
    m_input = std::make_shared<donkeycar::PartIO>();
}
Part::~Part()
{}
void Part::start()
{
    // start will create the thread so only if we're threaded
    if(m_threaded) {
        // will create the thread
        donkeycar::concurrency::ManagedThread::start();
        // set it to running
        donkeycar::concurrency::ManagedThread::resume();
    }
}

// Pattern for this method.  And hide whether or not the part is threaded
// within this implementation
PartData Part::run(const PartData input) {
    m_input = input;
    if(this->m_threaded) {
        // just get latest cached
        return m_output;
    } else {
        // directly call work method (also called by thread if threaded)
        update();
        return m_output;
    }
}

void Part::shutdown()
{
    if(m_threaded) {
        donkeycar::concurrency::ManagedThread::graceful_exit();
        donkeycar::concurrency::ManagedThread::join();
    }
}
const std::string& Part::output_topic() {return m_output_topic;}
const std::string& Part::input_topic() {return m_input_topic;}

} // namespace donkeycar