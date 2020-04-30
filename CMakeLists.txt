cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

project(CMSC630)

find_package(OpenCV REQUIRED)

set(Boost_USE_MULTITHREADED TRUE)
find_package(Boost 1.72 REQUIRED COMPONENTS filesystem regex)

#if(CMAKE_VERSION VERSION_LESS "2.8.11")
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${OpenCV_INCLUDE_DIRS})
#endif()

add_executable(cmsc603 src/main.cpp)
target_link_libraries(cmsc603 ${OpenCV_LIBS})
target_link_libraries(cmsc603 ${Boost_LIBRARIES})

add_executable(cmsc603KNN src/main_knn.cpp)
target_link_libraries(cmsc603KNN ${OpenCV_LIBS})
target_link_libraries(cmsc603KNN ${Boost_LIBRARIES})
