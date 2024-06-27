
cmake_minimum_required(VERSION 3.14)

# Set project name, version and languages here. (change as needed)
# Version numbers are available by including "exampleConfig.h" in
# the source. See exampleConfig.h.in for some more details.
project(CPP_BOILERPLATE VERSION 1.2.3.4 LANGUAGES CXX)
set(EXECUTABLE_NAME donkeyCarGT)

# Options: Things you can set via commandline options to cmake (e.g. -DENABLE_LTO=[ON|OFF])
option(ENABLE_WARNINGS_SETTINGS "Allow target_set_warnings to add flags and defines.
                                 Set this to OFF if you want to provide your own warning parameters." ON)
option(ENABLE_LTO "Enable link time optimization" ON)
option(ENABLE_DOCTESTS "Include tests in the library. Setting this to OFF will remove all doctest related code.
                        Tests in tests/*.cpp will still be enabled." ON)

# Include stuff. No change needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(ConfigSafeGuards)
include(Colors)
include(CTest)
include(Doctest)
include(Documentation)
include(LTO)
include(Misc)
include(Warnings)

# --------------------------------------------------------------------------------
#                         Locate Dependencies
# --------------------------------------------------------------------------------
# Check for LTO support
find_lto(CXX)
find_package(Threads REQUIRED)
# easiest: docker, if need to upgrade 
find_package(OpenCV 4.6.0 REQUIRED)
MESSAGE(STATUS "Include dirs ${OpenCV_INCLUDE_DIRS}")

# aptitude install libmosquitto-dev
# TODO: disable the NetworkMqtt part if not found
#find_package(Mosquitto REQUIRED)
#message(STATUS "libmosquitto: ${MOSQUITTO_INCLUDE_DIRS} ${MOSQUITTO_LIBRARIES}")

# --------------------------------------------------------------------------------
#                         Locate files
# --------------------------------------------------------------------------------
set(SOURCES          # All .cpp files in src/
    src/event.cpp
    src/mutex.cpp
    src/managed-task-status.cpp
    src/managed-thread.cpp
    src/vehicle.cpp
    src/parts/image.cpp
    src/parts/image-consumer.cpp
    src/parts/image-list-camera.cpp
    src/parts/line-follower.cpp
    src/parts/web-camera.cpp
    include/lock.hpp
    include/locked-queue.hpp
    include/locked-map.hpp
    include/memory.hpp
    include/vehicle.h
    include/parts/camera.hpp
    include/parts/image.h
    include/parts/image-consumer.h
    include/parts/image-list-camera.h
    include/parts/line-follower.h
    include/parts/network.hpp
    include/parts/part.hpp
    include/parts/part-io.h
    include/parts/web-camera.h
)
set(TESTFILES        # All .cpp files in tests/
    tests/main.cpp
)
set(LIBRARY_NAME engine)  # Default name for the library built from src/*.cpp (change if you wish)

# --------------------------------------------------------------------------------
#                            Build
# --------------------------------------------------------------------------------
# Compile all sources into a library.
add_library(${LIBRARY_NAME} OBJECT ${SOURCES})

# Lib needs its header files, and users of the library must also see these (PUBLIC). (No change needed)
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# There's also (probably) doctests within the library, so we need to see this as well.
target_link_libraries(${LIBRARY_NAME} PUBLIC 
    doctest
    Threads::Threads
    ${OpenCV_LIBS}
    stdc++fs
)

# Set the compile options you want (change as needed).
target_set_warnings(${LIBRARY_NAME} ENABLE ALL AS_ERROR ALL DISABLE Annoying)
# target_compile_options(${LIBRARY_NAME} ... )  # For setting manually.

# Add an executable for the file app/main.cpp.
# If you add more executables, copy these lines accordingly.
add_executable(${EXECUTABLE_NAME} app/main.cpp)   # Name of exec. and location of file.
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_NAME})  # Link the executable to library (if it uses it).
target_set_warnings(${EXECUTABLE_NAME} ENABLE ALL AS_ERROR ALL DISABLE Annoying) # Set warnings (if needed).
target_enable_lto(${EXECUTABLE_NAME} optimized)  # enable link-time-optimization if available for non-debug configurations

# Set the properties you require, e.g. what C++ standard to use. Here applied to library and main (change as needed).
set_target_properties(
    ${LIBRARY_NAME} ${EXECUTABLE_NAME}
      PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

# Set up tests (see tests/CMakeLists.txt).
add_subdirectory(tests)
