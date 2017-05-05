cmake_minimum_required(VERSION 3.6)
project(boundary_box)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(boundary_box ${SOURCE_FILES})

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries( boundary_box ${OpenCV_LIBS} )