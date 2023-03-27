cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED BOARD)
    set(BOARD "arduino_due")
endif()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(TINY-BLEP-CPP LANGUAGES CXX)

if(DEFINED RAINBOW_VERSION)
    target_compile_definitions(app PRIVATE RAINBOW_VERSION=${RAINBOW_VERSION})
endif()

if(DEFINED GF16_LOOKUP)
    target_compile_definitions(app PRIVATE GF16_LOOKUP=${GF16_LOOKUP})
endif()

if(DEFINED GF256_LOOKUP)
    target_compile_definitions(app PRIVATE GF256_LOOKUP=${GF256_LOOKUP})
endif()


target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src/)

# We are building as stand-alone project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_sources(app PRIVATE src/main.cpp)
endif()