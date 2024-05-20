
## DonkeyCarGT: Donkey Car, C++ and You 

This is the very early stage of an implementation of Donkey Car components in modern C++.

- [Donkey Car is a self-driving car platform for hobby remote-control cars](https://dbcs.donkeycar.com/).
- The structure of this repo is modeled after the [cpp-project template](https://github.com/bsamseth/cpp-project).
- Why? Good times.

|    |    |
:---:|:---:
![circuit](resource/donkeycarGT-circuit1.gif) | ![ucsd](resource/donkeycarGT-ucsd-outdoor.gif)



## Status
- [X] Initial polymorphic architecture between `Vehicle` and `Part`
- [X] Smart-pointer, polymorphic architecture `Part` input and output data.
- [X] Native, _managed_, thread support within Parts (a Part _is_ a pthread)
- [X] Thread-safe hashtable for Part I/O storage
- [X] Support for Camera Parts: `ImageListCamera` and a generic webcam `WebCamera` via OpenCV
- [ ] Throttle the `Vehicle` run loop to a specified frequency
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
