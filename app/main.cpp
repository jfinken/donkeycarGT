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
#include "parts/camera.h"
#include "parts/image-list-camera.h"

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
    
    donkeycar::Vehicle v;
    bool threaded = true;

    // Create a Camera Part
    //std::shared_ptr<donkeycar::Part> cam = 
    //    std::make_shared<donkeycar::Camera>("my_cam", "", "cam/image", threaded);

    // Create an ImageListCamera part 
    // TODO: read env for this path
    std::string path = "/home/jfinken/projects/40-49-autonomy/41-sensing-and-perception/41.12-donkeycar/mycar/data/ucsd_afternoon/images/";
    std::shared_ptr<donkeycar::Part> cam = 
        std::make_shared<donkeycar::ImageListCamera>(path, "my_cam", "", "cam/image", threaded);

    v.add(cam);
    v.start_blocking();

    return 0;
}
