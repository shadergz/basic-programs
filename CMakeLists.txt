cmake_minimum_required(VERSION 3.21)

project(Programs LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(common STATIC program.cpp common.h)

include_directories(.)
add_subdirectory(external/boost)

add_subdirectory(entropy)
add_subdirectory(hexdump)
add_subdirectory(today)

add_subdirectory(disasop)
add_subdirectory(bin2array)
