cmake_minimum_required(VERSION 3.10)
project(SpikingReservoir)

# Use C++17 (required for LibTorch)
set(CMAKE_CXX_STANDARD 17)

# Armadillo
find_package(Armadillo REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)

# include_directories(/usr/local/include)
# include_directories(/usr/include/eigen3)

# Add source files
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

include_directories("${PROJECT_SOURCE_DIR}/include")

# Create executable
add_executable(program ${SOURCES})

target_include_directories(program PRIVATE ${ARMADILLO_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})

# Link against LibTorch and other libraries
target_link_libraries(program PRIVATE armadillo raylib GL m pthread dl rt X11)

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    target_link_libraries(program PRIVATE OpenMP::OpenMP_CXX)
endif()

# Enable profiling with gprof
# terminal:: gprof program gmon.out | gprof2dot -w | dot -Tpng -o output.png
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -O0")

# Set rpath so the program can find the shared libraries
set_property(TARGET program PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_BUILD_RPATH "/opt/libtorch/lib")
