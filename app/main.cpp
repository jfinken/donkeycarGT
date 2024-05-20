// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#endif

#include <iostream>
#include <stdlib.h>

#include "exampleConfig.h"
#include "vehicle.h"
#include "parts/image-consumer.h"
#include "parts/image-list-camera.h"
#include "parts/network.hpp"
//#include "parts/web-camera.h"

std::string get_env_var( std::string const & key ) 
{
    char * val = std::getenv( key.c_str() );
    return val == NULL ? std::string("") : std::string(val);
}

// Accessing cmake definitions (here the version number) from source code.
int main() {
    std::cout << "DonkeycarGT: Donkeycar, C++, and you v"
              << PROJECT_VERSION_MAJOR
              << "."
              << PROJECT_VERSION_MINOR
              << "."
              << PROJECT_VERSION_PATCH
              << "."
              << PROJECT_VERSION_TWEAK
              << std::endl;

    // Instantiate the top-level Vehicle    
    donkeycar::Vehicle v;
    bool threaded = true;

    // Create an ImageListCamera part to generate imagery
    const std::string env_key = "DK_TUB_IMAGE_PATH";
    const std::string tub_image_path = get_env_var(env_key);
    const std::string cam_img_topic = "cam/image";
    const std::string pub_img_topic = "image";

    if(tub_image_path.empty()) {
        std::cerr << "Path to Tub imagery not found at: " << env_key << std::endl;
        exit(1);
    }
    std::shared_ptr<donkeycar::Part> file_cam =
        std::make_shared<donkeycar::ImageListCamera<donkeycar::Image>>
            (tub_image_path, "file-camera", "", cam_img_topic, threaded);

    /*
    // Create a WebCamera part to generate imagery
    int camera_id = 2;
    std::shared_ptr<donkeycar::Part> web_cam =
        std::make_shared<donkeycar::WebCamera>(camera_id, "web-camera", "", "cam/image", threaded);

    */
    // Create an Image part to consume imagery
    std::shared_ptr<donkeycar::Part> img =
        std::make_shared<donkeycar::ImageConsumer>("image-consumer", cam_img_topic, "", threaded);

    // Prototyping with a class template for the Network Part
    std::shared_ptr<donkeycar::Part> img_pub =
        std::make_shared<donkeycar::NetworkPublisherMqtt<donkeycar::Image>>(
            pub_img_topic, cam_img_topic, "", threaded
        );
    (void)img_pub;
    /*
    auto img_sub = std::make_shared<donkeycar::NetworkMqttSubscriber<donkeycar::Image>>();
    auto pose_pub = std::make_shared<donkeycar::NetworkMqttPublisher<donkeycar::Pose>>();
    auto pose_sub = std::make_shared<donkeycar::NetworkMqttSubscriber<donkeycar::Pose>>();
    */
    /*
    // std::shared_ptr<donkeycar::Part<donkeycar::PartIO>> mqtt_pub 
    //  ---^ Part is not templated...
    std::shared_ptr<donkeycar::Network<donkeycar::Image>> mqtt_pub =
        std::make_shared<donkeycar::Network<donkeycar::Image>>
            ("image-pub", "cam/image", "", threaded);
    */

    // Add the Parts to the Vehicle!
    v.add(file_cam);
    v.add(img);

    // And run!
    // The Vehicle ctor registers SIGINT (Ctrl-C) to exit this blocking method
    // and safely shutdown and join any threaded parts
    v.start_blocking();

    return 0;
}
