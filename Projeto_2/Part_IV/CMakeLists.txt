cmake_minimum_required(VERSION 3.10)

# Project name
project(Part_IV)

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

# Add executable for Predictor
add_executable(predictor PredictorTest.cpp Predictor.cpp Golomb.cpp BitStream.cpp)

# Add executable for IntraDecoderTest
add_executable(intra_decoder_test IntraDecoderTest.cpp BitStream.cpp IntraEncoder.cpp IntraDecoder.cpp Predictor.cpp Golomb.cpp)

# Add executable for IntraEncoderTest
add_executable(intra_encoder_test IntraEncoderTest.cpp BitStream.cpp IntraEncoder.cpp Predictor.cpp Golomb.cpp)

# Link OpenCV and SFML libraries
target_link_libraries(predictor ${OpenCV_LIBS} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
target_link_libraries(intra_decoder_test ${OpenCV_LIBS} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
target_link_libraries(intra_encoder_test ${OpenCV_LIBS} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)