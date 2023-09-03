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

if(ENABLE_TESTS)
    # rainbow
    add_library(
        blep_cpp_rainbowI_test
        math/gf16.cpp
        math/gf_operations.cpp
        schemes/rainbow.cpp
        utils/file.cpp
        utils/hash.cpp
        utils/rand.cpp
    )

    target_link_libraries(blep_cpp_rainbowI_test PUBLIC OpenSSL::SSL)


    add_library(
        blep_cpp_rainbowIII_test
        math/gf256.cpp
        math/gf_operations.cpp
        schemes/rainbow.cpp
        utils/file.cpp
        utils/hash.cpp
        utils/rand.cpp
    )

    target_link_libraries(blep_cpp_rainbowIII_test PUBLIC OpenSSL::SSL)


    add_library(
        blep_cpp_rainbowV_test
        math/gf256.cpp
        math/gf_operations.cpp
        schemes/rainbow.cpp
        utils/file.cpp
        utils/hash.cpp
        utils/rand.cpp
    )

    target_link_libraries(blep_cpp_rainbowV_test PUBLIC OpenSSL::SSL)
endif()