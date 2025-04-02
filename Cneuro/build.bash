#!/bin/bash

# Set build directory
BUILD_DIR="/home/medlar/SpikingReservoir/Cneuro/build"

# Create build directory if it doesn't exist
mkdir -p $BUILD_DIR

# Navigate to build directory
cd $BUILD_DIR

# Run CMake configuration
cmake ..

# Compile the project
make -j$(nproc)

# Run the program
./program
