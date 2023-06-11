add_executable(hash_test hash_test.c)

add_executable(matrix_test matrix_test.c)

add_executable(rainbow_test rainbow_test.c)
target_compile_definitions(rainbow_test PRIVATE PROJECT_DIR=${PROJECT_SOURCE_DIR} ELEM_TYPE=${ELEM_TYPE})

target_link_libraries(hash_test PUBLIC blep_c)
target_link_libraries(matrix_test PUBLIC blep_c_matrix_test)
target_link_libraries(rainbow_test PUBLIC blep_c)

add_test(hash_test hash_test)
add_test(matrix_test matrix_test)
add_test(rainbow_test rainbow_test)