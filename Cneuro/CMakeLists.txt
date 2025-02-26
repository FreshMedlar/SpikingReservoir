cmake_minimum_required(VERSION 3.10)
project(SpikingReservoir)

# Use C++17 (required for LibTorch)
set(CMAKE_CXX_STANDARD 17)

# Find LibTorch
set(Torch_DIR "/opt/libtorch")
find_package(Torch REQUIRED)

# Add source files
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

include_directories("${PROJECT_SOURCE_DIR}/include")

# Create executable
add_executable(program ${SOURCES})

# Link against LibTorch and other libraries
target_link_libraries(program "${TORCH_LIBRARIES}" raylib GL m pthread dl rt X11)

# Set rpath so the program can find the shared libraries
set_property(TARGET program PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_property(TARGET program PROPERTY CXX_STANDARD 17)
set(CMAKE_BUILD_RPATH "/opt/libtorch/lib")
