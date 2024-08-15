cmake_minimum_required(VERSION 3.21)

add_executable(entropy entry.cpp)

target_link_libraries(entropy common)
