add_executable(hash_test hash_test.cpp)
add_executable(matrix_test matrix_test.cpp)

add_executable(rainbow_test rainbow_test.cpp)
target_compile_definitions(rainbow_test PRIVATE RAINBOW_VERSION=${RAINBOW_VERSION} PROJECT_DIR=${PROJECT_SOURCE_DIR})

#add_executable(rainbowIII_test rainbow_test.cpp)
#target_compile_definitions(rainbowIII_test PRIVATE "RAINBOW_VERSION=2")

target_link_libraries(hash_test PUBLIC blep_cpp)
target_link_libraries(matrix_test PUBLIC blep_cpp)
target_link_libraries(rainbow_test PUBLIC blep_cpp)
#target_link_libraries(rainbowIII_test PUBLIC blep_cpp)

add_test(hash_ctest hash_test)
add_test(matrix_ctest matrix_test)
add_test(rainbow_ctest rainbow_test)
#add_test(rainbowIII_ctest rainbowIII_test)