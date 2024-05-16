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