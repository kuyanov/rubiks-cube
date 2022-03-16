cmake_minimum_required(VERSION 3.21)
project(rubiks-cube)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_ASAN "-g -fsanitize=address,undefined")

add_executable(RubiksCube rubiks-cube.cpp)
