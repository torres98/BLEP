if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

project(BLEP-C LANGUAGES C)
cmake_minimum_required(VERSION 3.22)

find_package(OpenSSL REQUIRED)

enable_testing()

if(NOT DEFINED RAINBOW_VERSION OR RAINBOW_VERSION STREQUAL "1")
    set(RAINBOW_VERSION "1")
    set(ELEM_TYPE "gf16")
    set(ELEM_BIT_SIZE "4")
    set(ELEM_ADD "gf16_add")
    set(ELEM_SUB "gf16_add")
    set(ELEM_MUL "gf16_mul")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -include ${PROJECT_SOURCE_DIR}/include/blep/math/gf16.h")
    add_definitions(-include ${PROJECT_SOURCE_DIR}/include/blep/math/gf16.h)
elseif(RAINBOW_VERSION STREQUAL "2" OR RAINBOW_VERSION STREQUAL "3")
    set(ELEM_TYPE "gf256")
    set(ELEM_BIT_SIZE "8")
    set(ELEM_ADD "gf256_add")
    set(ELEM_SUB "gf256_add")
    set(ELEM_MUL "gf256_mul")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -include ${PROJECT_SOURCE_DIR}/include/blep/math/gf256.h")
    add_definitions(-include ${PROJECT_SOURCE_DIR}/include/blep/math/gf256.h)
endif()

add_subdirectory(src/)
add_subdirectory(tests/)
add_subdirectory(scripts/)

target_compile_definitions(
    blep_c PUBLIC
    RAINBOW_VERSION=${RAINBOW_VERSION} ELEM_TYPE=${ELEM_TYPE} ELEM_BIT_SIZE=${ELEM_BIT_SIZE}
    ELEM_ADD=${ELEM_ADD} ELEM_SUB=${ELEM_SUB} ELEM_MUL=${ELEM_MUL})

if(DEFINED GF16_LOOKUP)
    target_compile_definitions(blep_c PUBLIC GF16_LOOKUP=${GF16_LOOKUP})
endif()

if(DEFINED GF256_LOOKUP)
    target_compile_definitions(blep_c PUBLIC GF256_LOOKUP=${GF256_LOOKUP})
endif()

target_include_directories(blep_c PUBLIC ${PROJECT_SOURCE_DIR}/include)

# just for matrix testing
target_compile_definitions(blep_c_matrix_test PUBLIC ELEM_TYPE=int ELEM_BIT_SIZE=32)

target_include_directories(blep_c_matrix_test PUBLIC ${PROJECT_SOURCE_DIR}/include)