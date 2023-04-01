cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_CXX_STANDARD 17)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(RAINBOW-BENCHMARK-CPP LANGUAGES CXX)

add_subdirectory(src/tiny_blep/)

if (BOARD MATCHES "^qemu")
    target_sources(app PRIVATE src/qemu_benchmark.cpp)
    target_compile_definitions(app PRIVATE SAMPLE_SIZE=${SAMPLE_SIZE} PROGRESSIVE_STEPS=${PROGRESSIVE_STEPS})
else()
    target_sources(app PRIVATE src/benchmark.cpp)
endif()