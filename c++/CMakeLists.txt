project(BLEP-C++ CXX)
cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

enable_testing()

add_subdirectory(src/)
add_subdirectory(tests/)
add_subdirectory(scripts/)

target_include_directories(blep_cpp PUBLIC ${PROJECT_SOURCE_DIR}/include)