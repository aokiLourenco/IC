cmake_minimum_required(VERSION 3.10)

# Project name
project(Aula01)

# Set the C++ standard to C++11 or higher
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Python3 and its components (NumPy and matplotlib)
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

# Include directories for Python and NumPy
include_directories(${Python3_INCLUDE_DIRS} ${Python3_NumPy_INCLUDE_DIRS})

set(MATPLOTLIBCPP_DIR ${PROJECT_SOURCE_DIR}/matplotlib-cpp)
include_directories(${MATPLOTLIBCPP_DIR})


# Find libsndfile
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED sndfile)

# Add the executable targets (main.cpp as the source files)
add_executable(P1 src/P1.cpp)
add_executable(P2 src/P2.cpp)

# Link Python libraries and libsndfile to the targets
target_link_libraries(P1 PRIVATE ${Python3_LIBRARIES} ${SNDFILE_LIBRARIES})
target_link_libraries(P2 PRIVATE ${Python3_LIBRARIES} ${SNDFILE_LIBRARIES})

# Include directories for libsndfile
target_include_directories(P1 PRIVATE ${SNDFILE_INCLUDE_DIRS})
target_include_directories(P2 PRIVATE ${SNDFILE_INCLUDE_DIRS})
