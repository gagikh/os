# Embedded Linux lectures: all C/C++ examples.
#
# Native build (on the board, or on a PC to test):
#     cmake -S . -B build
#     cmake --build build
#
# Cross build for the boards (on an x86 Linux PC or WSL2, after
# `sudo apt install g++-aarch64-linux-gnu`):
#     cmake -S . -B build-arm64 -DCMAKE_TOOLCHAIN_FILE=cmake/aarch64-linux-gnu.cmake
#     cmake --build build-arm64
#
# Binaries end up in build*/bin/.

cmake_minimum_required(VERSION 3.16)
project(embedded_linux_lectures LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
add_compile_options(-Wall -Wextra)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Day 1
add_executable(hello_c            day01/hello.c)
add_executable(hello_cpp          day01/hello.cpp)
add_executable(sysinfo            day01/template.cpp)
add_executable(sysinfo_solution   day01/solution/solution.cpp)
