target_sources(
    app PRIVATE
    math/gf16.cpp
    math/gf256.cpp
    math/gf_operations.cpp
    schemes/rainbow.cpp
    utils/hash.cpp
    utils/uart.cpp
    utils/SHA/sha384.c
    utils/SHA/sha512.c
)