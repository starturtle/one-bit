cmake_minimum_required(VERSION 3.10)
project( one-bit )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package( OpenCV REQUIRED )
add_executable( one-bit main.cpp )
target_link_libraries( one-bit ${OpenCV_LIBS} )