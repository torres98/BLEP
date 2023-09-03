if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

project(BLEP-C++ LANGUAGES CXX)
cmake_minimum_required(VERSION 3.22)

find_package(OpenSSL REQUIRED)

add_subdirectory(src/)
add_subdirectory(scripts/)

if(DEFINED RAINBOW_VERSION)
    target_compile_definitions(blep_cpp PUBLIC RAINBOW_VERSION=${RAINBOW_VERSION})
else()
    target_compile_definitions(blep_cpp PUBLIC RAINBOW_VERSION=1)
endif()

if(DEFINED GF16_LOOKUP)
    target_compile_definitions(blep_cpp PUBLIC GF16_LOOKUP=${GF16_LOOKUP})
endif()

if(DEFINED GF256_LOOKUP)
    target_compile_definitions(blep_cpp PUBLIC GF256_LOOKUP=${GF256_LOOKUP})
endif()

target_include_directories(blep_cpp PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests/)

    # for rainbow tests
    target_include_directories(blep_cpp_rainbowI_test PUBLIC ${PROJECT_SOURCE_DIR}/include)
    target_compile_definitions(blep_cpp_rainbowI_test PUBLIC RAINBOW_VERSION=1)

    target_include_directories(blep_cpp_rainbowIII_test PUBLIC ${PROJECT_SOURCE_DIR}/include)
    target_compile_definitions(blep_cpp_rainbowIII_test PUBLIC RAINBOW_VERSION=2)

    target_include_directories(blep_cpp_rainbowV_test PUBLIC ${PROJECT_SOURCE_DIR}/include)
    target_compile_definitions(blep_cpp_rainbowV_test PUBLIC RAINBOW_VERSION=3)
endif()