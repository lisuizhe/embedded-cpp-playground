cmake_minimum_required(VERSION 3.5.1)
project(ipc3)
add_executable(ipc3 ipc3.cpp)
target_link_libraries(ipc3 rt)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

SET(CMAKE_CXX_FLAGS "--std=c++11 -g")

set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabi-g++)