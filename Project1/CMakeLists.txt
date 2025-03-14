cmake_minimum_required(VERSION 3.10)

# Project name
project(1st_Class)

# Set the C++ standard to C++11 or higher
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Python3 and its components (NumPy and matplotlib)
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

# Include directories for Python and NumPy
include_directories(${Python3_INCLUDE_DIRS} ${Python3_NumPy_INCLUDE_DIRS})

# Set the path for matplotlibcpp
set(MATPLOTLIBCPP_DIR ${PROJECT_SOURCE_DIR}/matplotlib)
include_directories(${MATPLOTLIBCPP_DIR})

# Find the SFML package
find_package(SFML 2.5 COMPONENTS audio REQUIRED)

# Include directories for SFML
include_directories(${SFML_INCLUDE_DIRS})

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include directories for OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(P1 src/P1.cpp)
add_executable(P2 src/P2.cpp)
add_executable(P3 src/P3.cpp)

# Link Python libraries to T1 and T2 executables if they are separate
target_link_libraries(P1 PRIVATE ${Python3_LIBRARIES} sfml-audio)
target_link_libraries(P2 PRIVATE ${Python3_LIBRARIES} sfml-audio)
target_link_libraries(P3 PRIVATE ${OpenCV_LIBS} ${Python3_LIBRARIES})


