cmake_minimum_required(VERSION 3.10)

# Project name
project(Part_III)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(Headers)

# Find packages
find_package(OpenCV REQUIRED)
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

# Include OpenCV directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executable for Golomb encoder and tests
add_executable(audio_test audio.cpp Golomb.cpp BitStream.cpp)

# Link OpenCV and SFML libraries
target_link_libraries(audio_test ${OpenCV_LIBS} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)