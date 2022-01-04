cmake_minimum_required(VERSION 3.5.1)
project(atomic)
add_executable(atomic atomic.cpp)
target_link_libraries(atomic pthread)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

SET(CMAKE_CXX_FLAGS "--std=c++11")
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabi-g++)
