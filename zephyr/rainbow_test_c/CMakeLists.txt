# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rainbow_test)

if (${RAINBOW_VERSION} STREQUAL "1")
    set(GF "gf16")
elseif (${RAINBOW_VERSION} STREQUAL "2")
    set(GF "gf256")
    set(HASH_LIB "src/tiny/utils/SHA/sha384.c")
elseif (${RAINBOW_VERSION} STREQUAL "3")
    set(GF "gf256")
    set(HASH_LIB "src/tiny/utils/SHA/sha512.c")
else()
    MESSAGE(FATAL_ERROR "Rainbow version not recognized.")
endif()

add_definitions(-DRAINBOW_VERSION=${RAINBOW_VERSION})

add_definitions(-DELEM_TYPE=${GF})
add_definitions(-DELEM_ADD=${GF}_add)
add_definitions(-DELEM_SUB=${GF}_add)
add_definitions(-DELEM_MUL=${GF}_mul)

if(DEFINED ELEM_BIT_SIZE)
    add_definitions(-DELEM_BIT_SIZE=${ELEM_BIT_SIZE})
endif()

if (DEFINED GF16_LOOKUP)
    add_definitions(-DGF16_LOOKUP=${GF16_LOOKUP})
endif()

if (DEFINED GF256_LOOKUP)
    add_definitions(-DGF256_LOOKUP=${GF256_LOOKUP})
endif()

add_definitions(-include /home/torres/Desktop/Thesis/verification_implementation/zephyr/rainbow_test_c/src/tiny/include/${GF}.h)
#target_compile_definitions(app PRIVATE "-include /home/torres/Desktop/Thesis/verification_implementation/zephyr/rainbow_test_c/src/tiny/include/${GF}.h")


target_sources(app PRIVATE src/main.c src/tiny/schemes/rainbow.c src/tiny/utils/hash_utils.c src/tiny/utils/math_utils.c src/tiny/utils/memory_utils.c src/tiny/utils/random_utils.c src/tiny/utils/uart_utils.c src/tiny/utils/standard_verification.c src/tiny/utils/progressive_verification.c src/tiny/gf/gf_operations.c src/tiny/gf/gf16.c src/tiny/gf/${GF}.c ${HASH_LIB})
