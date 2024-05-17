
#include "doctest/doctest.h"
#include "parts/part.h"
#include <memory>
#include <thread>
#include <chrono>

namespace dk = donkeycar;

// Test class because Part is pure virtual
class BasicPart : public dk::Part 
{
public:
    BasicPart(const std::string part_name, const std::string& in_topic,
         const std::string& out_topic, const bool threaded = false)
    : Part(part_name, in_topic, out_topic, threaded)
    {}

    dk::PartData run(const dk::PartData input) override {
    
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

    void update() override
    {
        int exec_time_ms = 10;
        std::this_thread::sleep_for(std::chrono::milliseconds(exec_time_ms));
    }
};
TEST_CASE("part test: basic ctor")
{
    bool threaded = false;
    std::shared_ptr<dk::Part> part = 
        std::make_shared<BasicPart>("basic-part", "foo", "bar", threaded);

    // basic checks on non-nullptrs for I/O data 

    // Note extra parens suppress operator<< overload compilation errors
    // https://github.com/doctest/doctest/issues/356
    CHECK((part->get_input_ptr() != nullptr));
    CHECK((part->get_output_ptr() != nullptr));

    std::string in_topic = part->input_topic();
    std::string out_topic = part->output_topic();
    CHECK((in_topic == "foo"));
    CHECK((out_topic == "bar"));

    dk::PartData output = part->run(part->get_input_ptr());
    CHECK((part->get_output_ptr() != nullptr));
}