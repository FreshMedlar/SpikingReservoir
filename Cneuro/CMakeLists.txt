cmake_minimum_required(VERSION 3.10)
project(SpikingReservoir)

# Use C++17 (required for LibTorch)
set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}/include")

# Add source files
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Create executable
add_executable(program ${SOURCES})

target_link_libraries(program PRIVATE raylib GL m pthread dl rt X11)

find_package(Eigen3 REQUIRED)
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    target_link_libraries(program PRIVATE OpenMP::OpenMP_CXX)
endif()

# Enable profiling with gprof
# terminal:: gprof program gmon.out | gprof2dot -w | dot -Tpng -o output.png
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")#-pg -O0")

# Set rpath so the program can find the shared libraries
set_property(TARGET program PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
