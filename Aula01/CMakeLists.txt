cmake_minimum_required(VERSION 3.10)

# Project name
project(Aula01)

# Set the C++ standard to C++11 or higher
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Matplot++ (assuming it has been installed globally)
find_package(Matplot++ REQUIRED)

# Find libsndfile
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED sndfile)

# Add the executable target (main.cpp as the source file)
add_executable(P1 src/P1.cpp)
add_executable(P2 src/P2.cpp)


# Link Matplot++ and libsndfile to the target
target_link_libraries(P1 PRIVATE Matplot++::matplot ${SNDFILE_LIBRARIES})

# Include directories for libsndfile
target_include_directories(P1 PRIVATE ${SNDFILE_INCLUDE_DIRS})
