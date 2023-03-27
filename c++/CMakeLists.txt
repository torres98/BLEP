project(BLEP-C++ CXX)
cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

enable_testing()

add_subdirectory(src/)
add_subdirectory(tests/)
add_subdirectory(scripts/)

if(DEFINED RAINBOW_VERSION)
    target_compile_definitions(blep_cpp PRIVATE RAINBOW_VERSION=${RAINBOW_VERSION})
endif()

if(DEFINED GF16_LOOKUP)
    target_compile_definitions(blep_cpp PRIVATE GF16_LOOKUP=${GF16_LOOKUP})
endif()

if(DEFINED GF256_LOOKUP)
    target_compile_definitions(blep_cpp PRIVATE GF256_LOOKUP=${GF256_LOOKUP})
endif()

target_include_directories(blep_cpp PUBLIC ${PROJECT_SOURCE_DIR}/include)