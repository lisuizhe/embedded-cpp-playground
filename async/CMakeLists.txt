cmake_minimum_required(VERSION 3.5.1)
project(async)
add_executable(async async.cpp)
target_link_libraries(async pthread -static-libstdc++)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_CXX_FLAGS "--std=c++14")
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabi-g++)