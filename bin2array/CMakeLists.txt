cmake_minimum_required(VERSION 3.21)

add_executable(bin2array program.cpp)
target_link_libraries(bin2array Boost::program_options)