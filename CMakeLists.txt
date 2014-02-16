cmake_minimum_required(VERSION 2.8)

project(config-reader)
add_executable(read.it main.cpp)
target_link_libraries(read.it lua5.1)
add_definitions(-Wall -g -O0)
