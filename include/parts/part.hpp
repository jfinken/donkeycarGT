#pragma once

#include "part-io.h"
#include "managed-thread.h"

typedef std::shared_ptr<donkeycar::PartIO> PartData;

namespace donkeycar {

// Part defines an interface invoked via Vehicle.  TODO: more doc
//template<typename MessageT>
class Part : public donkeycar::concurrency::ManagedThread
{
public:
    Part(const std::string part_name, const std::string& in_topic,
        const std::string& out_topic, const bool threaded)
    : donkeycar::concurrency::ManagedThread(part_name, true)
    , m_input_topic(in_topic)
    , m_output_topic(out_topic)
    , m_threaded(threaded) 
    {
        // Note: if threaded, start paused

        m_part_name = part_name;
        //m_output = std::make_shared<MessageT>();
        //m_input = std::make_shared<MessageT>();
    }
    
    virtual ~Part(){}
    
    virtual void start() override
    {
        // start will create the thread so only if we're threaded
        if(m_threaded) {
            // will create the thread
            donkeycar::concurrency::ManagedThread::start();
            // set it to running
            donkeycar::concurrency::ManagedThread::resume();
        }
    }

    virtual PartData run(const PartData input = nullptr) = 0;
    /*
    {
        (void)input;
        return nullptr;
    }
    */
    
    virtual void update() = 0;
    
    virtual void shutdown()
    {
        if(m_threaded) {
            donkeycar::concurrency::ManagedThread::graceful_exit();
            donkeycar::concurrency::ManagedThread::join();
        }
    }

    // API for Part input and outputs
    //virtual PartData get_input_ptr() {return m_input;}; 
    //virtual PartData get_output_ptr() {return m_output;}

    const std::string& output_topic(){return m_output_topic;}
    const std::string& input_topic() {return m_input_topic;}

protected:
    // TODO?
    //PartData m_output;
    //PartData m_input;

    std::string m_input_topic;
    std::string m_output_topic;
    std::string m_part_name;
    bool m_threaded;
};
/*
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//template<typename MessageT>
//Part<MessageT>::Part(const std::string part_name, const std::string& in_topic,
Part::Part(const std::string part_name, const std::string& in_topic,
    const std::string& out_topic, const bool threaded) 
    : donkeycar::concurrency::ManagedThread(part_name, true)
    , m_input_topic(in_topic)
    , m_output_topic(out_topic)
    , m_threaded(threaded) 
{
    // Note: if threaded, start paused

    m_part_name = part_name;
    //m_output = std::make_shared<MessageT>();
    //m_input = std::make_shared<MessageT>();
}

//template<typename MessageT>
//void Part<MessageT>::start()
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

// Implementation classes must implement run() and update()
// Note, no run_threaded API.  If threaded, then the Part will
// have already called update.
//template<typename MessageT>
//std::shared_ptr<MessageT> 
//Part<MessageT>::run(const PartData input) {
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
//template<typename MessageT>
//void Part<MessageT>::shutdown()
void Part::shutdown()
{
    if(m_threaded) {
        donkeycar::concurrency::ManagedThread::graceful_exit();
        donkeycar::concurrency::ManagedThread::join();
    }
}

const std::string& Part::output_topic() {return m_output_topic;}

const std::string& Part::input_topic() {return m_input_topic;}
*/
} // namespace donkeycar