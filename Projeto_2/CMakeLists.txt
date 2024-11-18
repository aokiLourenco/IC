cmake_minimum_required(VERSION 3.10)

# Project name
project(BitStreamProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(Headers)

# Add executable for encoder
add_executable(encoder Encoder.cpp BitStream.cpp)

# Add executable for decoder
add_executable(decoder Decoder.cpp BitStream.cpp)