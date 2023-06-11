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

# just for matrix test
add_library(
    blep_c_matrix_test
    math/matrix.c
    utils/memory.c
    utils/rand.c
)