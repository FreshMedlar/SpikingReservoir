#!/bin/bash

# Set build directory
BUILD_DIR="build"

# Create build directory if it doesn't exist
mkdir -p $BUILD_DIR

# Navigate to build directory
cd $BUILD_DIR

# Run CMake configuration
cmake -DCMAKE_PREFIX_PATH=/opt/libtorch ..

# Compile the project
make -j$(nproc)

# Run the program
./program
