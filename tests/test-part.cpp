#include <chrono>
#include <memory>
#include <thread>
#include "doctest/doctest.h"
#include "parts/part.hpp"

class MyPartType : public donkeycar::PartIO {
   public:
    void set_data(const std::string s) { data_ = s; }
    std::string data() { return data_; }

   protected:
    std::string data_{""};
};
// Test class because Part is pure virtual
template <typename T>
class BasicPart : public donkeycar::Part {
   public:
    BasicPart(const std::string part_name, const std::string& in_topic,
              const std::string& out_topic, const bool threaded = false)
        : Part(part_name, in_topic, out_topic, threaded) {
        input_ = std::make_shared<T>();
        output_ = std::make_shared<T>();
    }

    virtual PartData run(const PartData input) override {
        input_ = std::static_pointer_cast<T>(input);
        if (this->m_threaded) {
            // just get latest cached
            return output_;
        } else {
            // directly call work method (also called by thread if threaded)
            update();
            return output_;
        }
    }

    virtual void update() override {
        int exec_time_ms = 10;
        std::this_thread::sleep_for(std::chrono::milliseconds(exec_time_ms));

        // process some data...
        output_->set_data("fancy-data");
    }

   protected:
    std::shared_ptr<T> input_;
    std::shared_ptr<T> output_;
};

TEST_CASE("part test: basic ctor") {
    bool threaded = false;
    std::shared_ptr<donkeycar::Part> part =
        std::make_shared<BasicPart<MyPartType>>("basic-part", "foo", "bar",
                                                threaded);

    SUBCASE("part test: basic ctor") {
        // Note extra parens suppress operator<< overload compilation errors
        // https://github.com/doctest/doctest/issues/356

        std::string in_topic = part->input_topic();
        std::string out_topic = part->output_topic();
        CHECK((in_topic == "foo"));
        CHECK((out_topic == "bar"));
    }
    // basic checks on non-nullptrs for I/O data
    SUBCASE("part test: basic ctor") {
        PartData in;
        PartData out = part->run(in);

        std::shared_ptr<MyPartType> d =
            std::static_pointer_cast<MyPartType>(out);
        printf("data: %s\n", d->data().c_str());
        CHECK((d->data() == "fancy-data"));
    }
}