add_library(
    blep_cpp
    math/gf16.cpp
    math/gf256.cpp
    math/gf_operations.cpp
    schemes/rainbow.cpp
    utils/file.cpp
    utils/hash.cpp
    utils/rand.cpp
)

target_link_libraries(blep_cpp PUBLIC OpenSSL::SSL)

# for rainbow tests

add_library(
    blep_rainbowI_test_cpp
    math/gf16.cpp
    math/gf_operations.cpp
    schemes/rainbow.cpp
    utils/file.cpp
    utils/hash.cpp
    utils/rand.cpp
)

target_link_libraries(blep_rainbowI_test_cpp PUBLIC OpenSSL::SSL)


add_library(
    blep_rainbowIII_test_cpp
    math/gf256.cpp
    math/gf_operations.cpp
    schemes/rainbow.cpp
    utils/file.cpp
    utils/hash.cpp
    utils/rand.cpp
)

target_link_libraries(blep_rainbowIII_test_cpp PUBLIC OpenSSL::SSL)


add_library(
    blep_rainbowV_test_cpp
    math/gf256.cpp
    math/gf_operations.cpp
    schemes/rainbow.cpp
    utils/file.cpp
    utils/hash.cpp
    utils/rand.cpp
)

target_link_libraries(blep_rainbowV_test_cpp PUBLIC OpenSSL::SSL)