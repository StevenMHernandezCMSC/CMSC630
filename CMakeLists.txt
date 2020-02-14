cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

project(CMSC630)

find_package(OpenCV REQUIRED)

if(CMAKE_VERSION VERSION_LESS "2.8.11")
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

add_executable(cmsc603 src/main.cpp)

target_link_libraries(cmsc603 ${OpenCV_LIBS})