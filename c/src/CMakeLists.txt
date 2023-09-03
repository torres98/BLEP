add_library(
    blep_c
    math/gf16.c
    math/gf256.c
    math/gf_operations.c
    math/matrix.c
    mv_verification/eff_ver.c
    mv_verification/prog_ver.c
    mv_verification/std_ver.c
    schemes/rainbow.c
    utils/file.c
    utils/hash.c
    utils/memory.c
    utils/rand.c
)

target_link_libraries(blep_c PUBLIC OpenSSL::SSL)

if(ENABLE_TESTS)
    # rainbow
    add_library(
        blep_c_rainbowI_test
        math/gf16.c
        math/gf_operations.c
        math/matrix.c
        mv_verification/eff_ver.c
        mv_verification/prog_ver.c
        mv_verification/std_ver.c
        schemes/rainbow.c
        utils/file.c
        utils/hash.c
        utils/memory.c
        utils/rand.c
    )

    target_link_libraries(blep_c_rainbowI_test PUBLIC OpenSSL::SSL)


    add_library(
        blep_c_rainbowIII_test
        math/gf16.c
        math/gf256.c
        math/gf_operations.c
        math/matrix.c
        mv_verification/eff_ver.c
        mv_verification/prog_ver.c
        mv_verification/std_ver.c
        schemes/rainbow.c
        utils/file.c
        utils/hash.c
        utils/memory.c
        utils/rand.c
    )

    target_link_libraries(blep_c_rainbowIII_test PUBLIC OpenSSL::SSL)


    add_library(
        blep_c_rainbowV_test
        math/gf16.c
        math/gf256.c
        math/gf_operations.c
        math/matrix.c
        mv_verification/eff_ver.c
        mv_verification/prog_ver.c
        mv_verification/std_ver.c
        schemes/rainbow.c
        utils/file.c
        utils/hash.c
        utils/memory.c
        utils/rand.c
    )

    target_link_libraries(blep_c_rainbowV_test PUBLIC OpenSSL::SSL)

    # matrix
    add_library(
        blep_c_matrix_test
        math/matrix.c
        utils/memory.c
        utils/rand.c
    )
endif()