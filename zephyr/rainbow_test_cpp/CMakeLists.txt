# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rainbow_test_cpp)

if(${RAINBOW_VERSION} STREQUAL "1")
    set(GF_LIB "src/tiny/gf/gf16.cpp")
elseif(${RAINBOW_VERSION} STREQUAL "2")
    set(GF_LIB "src/tiny/gf/gf256.cpp")
    set(HASH_LIB "src/tiny/utils/SHA/sha384.c")
elseif(${RAINBOW_VERSION} STREQUAL "3")
    set(GF_LIB "src/tiny/gf/gf256.cpp")
    set(HASH_LIB "src/tiny/utils/SHA/sha512.c")
else()
    MESSAGE(FATAL_ERROR "Rainbow version not recognized.")
endif()

add_definitions(-DRAINBOW_VERSION=${RAINBOW_VERSION})

if(DEFINED GF16_LOOKUP)
    add_definitions(-DGF16_LOOKUP=${GF16_LOOKUP})
endif()

if(DEFINED GF256_LOOKUP)
    add_definitions(-DGF256_LOOKUP=${GF256_LOOKUP})
endif()

target_sources(app PRIVATE src/main.cpp src/tiny/schemes/rainbow.cpp src/tiny/utils/uart_utils.cpp src/tiny/utils/hash_utils.cpp ${GF_LIB} ${HASH_LIB})
