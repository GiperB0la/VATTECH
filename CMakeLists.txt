cmake_minimum_required(VERSION 3.10)
project(Vattech)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(Vattech/include)

add_executable(Vattech Vattech/src/Main.cpp Vattech/src/Work.cpp)