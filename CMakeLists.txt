cmake_minimum_required(VERSION 3.16)
project(my_debugger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(debugger
    src/main.cpp
    src/debugger.cpp
    src/breakpoint.cpp
)

target_include_directories(debugger PRIVATE src)

add_executable(test_program tests/test_program.cpp)

target_compile_options(test_program PRIVATE -g -O0)
target_link_options(test_program PRIVATE -no-pie)
