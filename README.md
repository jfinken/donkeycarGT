
## DonkeyCarGT: Donkey Car, C++ and You 

This is the very early stage of an iterative port of Donkey Car components from python3x to modern C++.

- [Donkey Car is a self-driving car platform for hobby remote-control cars](https://dbcs.donkeycar.com/).
- The structure of this repo is modeled after the [cpp-project template](https://github.com/bsamseth/cpp-project).
- Why? Good times.

## Status
- [X] Initial polymorphic architecture between `Vehicle` and `Parts`
- [X] Native, _managed_, thread support within Parts (a Part _is_ a pthread)
- [X] Thread-safe hashtable for Part I/O storage
- [ ] Support for Camera Parts (beginning with `ImageListCamera` and generic webcam support with OpenCV)
- [ ] Support for Network Parts (mqtt and zmq with pub/sub semantics)
- [ ] Support for Tub_v2
- [ ] ...

## Building

Build by making a build directory (i.e. `build/`), run `cmake` in that dir, and then use `make` to build the desired target.

Example:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=[Debug | Coverage | Release]
make
./donkeyCarGT
make test      # Makes and runs the tests.
make coverage  # Generate a coverage report.
make doc       # Generate html documentation.
```
