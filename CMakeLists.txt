cmake_minimum_required(VERSION 3.19)
project(prog)

set(CMAKE_CXX_STANDARD 14)

add_executable(prog main.cpp)
target_compile_options(prog PUBLIC -Wall -Wextra -Wpedantic)