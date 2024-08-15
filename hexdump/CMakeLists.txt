cmake_minimum_required(VERSION 3.21)

add_executable(hexdump program.cpp)
target_link_libraries(hexdump PRIVATE Boost::program_options)