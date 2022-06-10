# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rainbow_benchmark)

if (${VERSION} STREQUAL "1")
    set(GF_LIB "src/tiny/gf/gf16.cpp")
elseif (${VERSION} STREQUAL "2")
    set(GF_LIB "src/tiny/gf/gf256.cpp")
    set(HASH_LIB "src/tiny/utils/sha384.c")
elseif (${VERSION} STREQUAL "3")
    set(GF_LIB "src/tiny/gf/gf256.cpp")
    set(HASH_LIB "src/tiny/utils/sha512.c")
else()
    MESSAGE(FATAL_ERROR "Rainbow version not recognized.")
endif()

add_definitions(-DVERSION=${VERSION})
add_definitions(-DSAMPLE_SIZE=${SAMPLE_SIZE})
add_definitions(-DSVK_NROWS=${SVK_NROWS})
add_definitions(-DPROGRESSIVE_STEPS=${PROGRESSIVE_STEPS})

target_sources(app PRIVATE src/main.cpp src/tiny/utils/uart_utils.cpp src/tiny/utils/hash_utils.cpp ${GF_LIB} ${HASH_LIB})
