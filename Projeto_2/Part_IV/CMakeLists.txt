cmake_minimum_required(VERSION 3.10)

# Project name
project(Part_IV)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/headers
)

# Find packages
find_package(OpenCV REQUIRED)
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

# Include OpenCV directories
include_directories(${OpenCV_INCLUDE_DIRS})

set(COMMON_SOURCES
    src/codecs/BitStream.cpp
    src/codecs/Converter.cpp
    src/codecs/Golomb.cpp
    src/codecs/Predictor.cpp
)

set(CODEC_SOURCES
    src/codecs/IntraEncoder.cpp
    src/codecs/IntraDecoder.cpp
    src/codecs/InterEncoder.cpp
    src/codecs/InterDecoder.cpp
)

# # Add executable for Predictor
# add_executable(predictor PredictorTest.cpp Predictor.cpp Golomb.cpp BitStream.cpp)

# # Add executable for IntraTester
# add_executable(intra_Decoder_tester IntraDecoderTest.cpp BitStream.cpp IntraDecoder.cpp Predictor.cpp Golomb.cpp Converter.cpp)

# add_executable(intra_Encoder_tester IntraEncoderTest.cpp BitStream.cpp IntraEncoder.cpp Predictor.cpp Golomb.cpp Converter.cpp)

# add_executable(inter_Decoder_tester
#     InterDecoderTest.cpp

#     InterDecoder.cpp 
#     IntraDecoder.cpp
#     BitStream.cpp 
#     Predictor.cpp 
#     Golomb.cpp 
#     Converter.cpp
# )

# add_executable(inter_Encoder_tester
#     InterEncoderTest.cpp

#     InterEncoder.cpp
#     IntraEncoder.cpp 
#     BitStream.cpp 
#     Predictor.cpp 
#     Golomb.cpp 
#     Converter.cpp
# )

add_executable(Lossy_tester
    src/LossyVideoCodec.cpp

    src/codecs/IntraEncoder.cpp
    src/codecs/IntraDecoder.cpp
    src/codecs/BitStream.cpp 
    src/codecs/Predictor.cpp 
    src/codecs/Golomb.cpp 
    src/codecs/Converter.cpp
)

add_executable(video_codec 
    src/main.cpp
    ${CODEC_SOURCES}
    ${COMMON_SOURCES}
)

# Link OpenCV and SFML libraries
# target_link_libraries(predictor ${OpenCV_LIBS} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
# target_link_libraries(intra_Decoder_tester ${OpenCV_LIBS} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
# target_link_libraries(intra_Encoder_tester ${OpenCV_LIBS} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
# target_link_libraries(inter_Decoder_tester ${OpenCV_LIBS} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
# target_link_libraries(inter_Encoder_tester ${OpenCV_LIBS} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
target_link_libraries(Lossy_tester ${OpenCV_LIBS} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
target_link_libraries(video_codec ${OpenCV_LIBS} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)