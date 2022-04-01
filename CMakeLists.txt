cmake_minimum_required(VERSION 3.21)
project(rubiks-cube)

set(CMAKE_C_FLAGS_ASAN "-g -fsanitize=address,undefined")
set(CMAKE_C_FLAGS_RELEASE "-Ofast -mtune=native")

add_executable(RubiksCube rubiks-cube.c)
