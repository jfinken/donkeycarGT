// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#endif

#include <stdlib.h>
#include <iostream>

#include "exampleConfig.h"
#include "parts/image-consumer.h"
#include "parts/image-list-camera.h"
#include "parts/line-follower.h"
#include "parts/network.hpp"
#include "parts/web-camera.h"
#include "vehicle.h"

std::string get_env_var(std::string const& key) {
    char* val = std::getenv(key.c_str());
    return val == NULL ? std::string("") : std::string(val);
}

int main() {
    // Accessing cmake definitions (version number) from source
    std::cout << "DonkeycarGT: Donkeycar, C++, and you v"
              << PROJECT_VERSION_MAJOR << "." << PROJECT_VERSION_MINOR << "."
              << PROJECT_VERSION_PATCH << "." << PROJECT_VERSION_TWEAK << "\n"
              << "OpenCV: " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION
              << std::endl;

    // Instantiate the top-level Vehicle
    donkeycar::Vehicle v;

    // Whether or not a Part is threaded
    bool threaded = true;

    // Create an ImageListCamera part to generate imagery
    const std::string env_key = "DK_TUB_IMAGE_PATH";
    const std::string tub_image_path = get_env_var(env_key);
    const std::string cam_img_topic = "cam/image";
    const std::string pub_img_topic = "image";

    if (tub_image_path.empty()) {
        std::cerr << "Path to Tub imagery not found at: " << env_key
                  << std::endl;
        exit(1);
    }
    std::shared_ptr<donkeycar::Part> file_cam =
        std::make_shared<donkeycar::ImageListCamera<donkeycar::Image>>(
            tub_image_path, "file-camera", "", cam_img_topic, threaded);
    /*
    // Or create a WebCamera part to generate imagery
    int camera_id = 2;
    std::shared_ptr<donkeycar::Part> web_cam =
        std::make_shared<donkeycar::WebCamera<donkeycar::Image>>(
            camera_id, "web-camera", "", cam_img_topic, threaded);
    */

    /*
    // Create an Image part to consume imagery
    std::shared_ptr<donkeycar::Part> img_consumer =
        std::make_shared<donkeycar::ImageConsumer>("image-consumer",
                                                   cam_img_topic, "", threaded);
    */

    /*
    // Prototyping the Network Part (WIP)
    std::shared_ptr<donkeycar::Part> img_pub =
        std::make_shared<donkeycar::NetworkPublisherMqtt<donkeycar::Image>>(
            pub_img_topic, cam_img_topic, "", threaded);
    (void)img_pub;
    */

    // Computer vision: line-follower
    std::shared_ptr<donkeycar::Part> vision =
        std::make_shared<donkeycar::vision::LineFollower>(
            "vision", cam_img_topic, "", threaded);
    
    // Add the Parts to the Vehicle!
    v.add(file_cam);
    v.add(vision);
    //v.add(img_consumer);

    // And run!
    // The Vehicle ctor registers SIGINT (Ctrl-C) to exit this blocking method
    // and safely shutdown and join any threaded parts
    v.start_blocking();

    return 0;
}
