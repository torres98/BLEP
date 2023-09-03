add_executable(hash_test hash_test.c)
target_link_libraries(hash_test PRIVATE blep_c)
add_test(hash_test hash_test)

add_executable(matrix_test matrix_test.c)
target_link_libraries(matrix_test PRIVATE blep_c_matrix_test)
add_test(matrix_test matrix_test)

add_executable(rainbowI_test rainbow_test.c)
target_compile_definitions(rainbowI_test PRIVATE PROJECT_DIR=${PROJECT_SOURCE_DIR})
target_link_libraries(rainbowI_test PUBLIC blep_c_rainbowI_test)
add_test(rainbowI_test rainbowI_test)

add_executable(rainbowIII_test rainbow_test.c)
target_compile_definitions(rainbowIII_test PRIVATE PROJECT_DIR=${PROJECT_SOURCE_DIR})
target_link_libraries(rainbowIII_test PUBLIC blep_c_rainbowIII_test)
add_test(rainbowIII_test rainbowIII_test)

add_executable(rainbowV_test rainbow_test.c)
target_compile_definitions(rainbowV_test PRIVATE PROJECT_DIR=${PROJECT_SOURCE_DIR})
target_link_libraries(rainbowV_test PUBLIC blep_c_rainbowV_test)
add_test(rainbowV_test rainbowV_test)