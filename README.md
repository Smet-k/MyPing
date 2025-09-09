# MyPing
## Overview
`MyPing` is a network utility that tests reachability of the host and measures round-trip time
## Installation
### Requirements
- Linux-Based OS
- C Compiler
### Build
```
# Clone the repository
https://github.com/Smet-k/MyPing
cd MyPing

# Build the utility
./build.sh

# Run
sudo ./build/MyPing
```
## Usage
```
MyPing [-d delay] destination
```

where:

 - \-d, --delay: Set delay(1-255) in seconds for pings
 - \-h, --help: Shows a help message