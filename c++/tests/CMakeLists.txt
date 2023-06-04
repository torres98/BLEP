add_executable(hash_test hash_test.cpp)
add_executable(matrix_test matrix_test.cpp)

add_executable(rainbow_test rainbow_test.cpp)
target_compile_definitions(rainbow_test PRIVATE PROJECT_DIR=${PROJECT_SOURCE_DIR})

target_link_libraries(hash_test PUBLIC blep_cpp)
target_link_libraries(matrix_test PUBLIC blep_cpp)
target_link_libraries(rainbow_test PUBLIC blep_cpp)

add_test(hash_ctest hash_test)
add_test(matrix_ctest matrix_test)
add_test(rainbow_ctest rainbow_test)