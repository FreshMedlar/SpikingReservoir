#!/bin/bash

# Set build directory
BUILD_DIR="build"

# Create build directory if it doesn't exist
mkdir -p $BUILD_DIR

# Navigate to build directory
cd $BUILD_DIR

# Compile the project using g++ (or another compiler)
g++ ~/SpikingReservoir/Cneuro/*.cpp -o program -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Run the program (optional)
./program
