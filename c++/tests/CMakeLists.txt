add_executable(hash_test hash_test.cpp)
target_link_libraries(hash_test PUBLIC blep_cpp)
add_test(hash_ctest hash_test)

add_executable(matrix_test matrix_test.cpp)
target_link_libraries(matrix_test PUBLIC blep_cpp)
add_test(matrix_ctest matrix_test)

add_executable(rainbowI_test rainbow_test.cpp)
target_compile_definitions(rainbowI_test PUBLIC PROJECT_DIR=${PROJECT_SOURCE_DIR})
target_link_libraries(rainbowI_test PUBLIC blep_rainbowI_test_cpp)
add_test(rainbowI_test rainbowI_test)

add_executable(rainbowIII_test rainbow_test.cpp)
target_compile_definitions(rainbowIII_test PUBLIC PROJECT_DIR=${PROJECT_SOURCE_DIR})
target_link_libraries(rainbowIII_test PUBLIC blep_rainbowIII_test_cpp)
add_test(rainbowIII_test rainbowIII_test)

add_executable(rainbowV_test rainbow_test.cpp)
target_compile_definitions(rainbowV_test PUBLIC PROJECT_DIR=${PROJECT_SOURCE_DIR})
target_link_libraries(rainbowV_test PUBLIC blep_rainbowV_test_cpp)
add_test(rainbowV_test rainbowV_test)