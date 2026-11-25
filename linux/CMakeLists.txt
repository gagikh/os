# Linux course: every day's template.
#
#     cmake -S . -B build
#     cmake --build build
#     ./build/bin/day01
#
# Each day builds one program, named after the day: day01, day02, ...

cmake_minimum_required(VERSION 3.16)
project(linux_course LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
add_compile_options(-Wall -Wextra)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add a day here when its folder exists.
foreach(day day01 day02 day03 day04 day05 day07 day08 day09 day10 day11)
    add_executable(${day} ${day}/template.cpp)
endforeach()
