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

if(DEFINED GF16_LOOKUP)
    target_compile_definitions(blep_cpp PRIVATE GF16_LOOKUP=${GF16_LOOKUP})
endif()

if(DEFINED GF256_LOOKUP)
    target_compile_definitions(blep_cpp PRIVATE GF256_LOOKUP=${GF256_LOOKUP})
endif()

target_link_libraries(blep_cpp PUBLIC OpenSSL::SSL)