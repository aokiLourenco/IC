cmake_minimum_required(VERSION 3.10)

# Project name
project(Part_II)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(Headers)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include OpenCV directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executable for Golomb encoder and tests
add_executable(golomb_tests GolombTests.cpp Golomb.cpp BitStream.cpp)

# Link OpenCV libraries
target_link_libraries(golomb_tests ${OpenCV_LIBS})