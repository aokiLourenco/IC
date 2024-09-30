cmake_minimum_required(VERSION 3.10)

# Set the project name
project(AudioProcessing)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable for part1
add_executable(part1 part1.cpp)

# Add the executable for part2
add_executable(part2 part2.cpp)

# Find the SFML package
find_package(SFML 2.5 COMPONENTS audio REQUIRED)

# Link SFML libraries to part2
target_link_libraries(part2 sfml-audio)