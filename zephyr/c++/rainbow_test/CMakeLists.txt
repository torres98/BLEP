cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_CXX_STANDARD 17)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(RAINBOW-TEST-CPP LANGUAGES CXX)

add_subdirectory(src/tiny_blep/)

target_sources(app PRIVATE src/rainbow_test.cpp)