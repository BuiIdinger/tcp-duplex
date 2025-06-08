# TCP-Duplex

A extremely high performance TCP duplex library, optimized for high input and output for real-time applications that cannot afford losing millisecond.

# Use cases

TCP-duplex should be used for extremely high performance applications where every millisecond counts.

TCP-Duplex eliminates modern C++ features like std::shared_ptr in order to favour performance and a low memory mark, which will require more code maintenance and will require more work in order to produce safe code.

# Features

- Duplex
- TLS
- Topics
- Automatic heartbeats
