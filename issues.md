**C++**
-maybe fix size of mlen for sha

-Use lookup tables in gf_operations file

-Make fill_matrix a friend function or directly a class method

**C**
-Fix rainbow generating 1 byte for gf16

- Improve rref performance

-For getters and setters: you need to consider the ratio between the actual size of the type and the user-specified size
    - maybe use unions and bitfields?

-Maybe add a macro containing the zero value for ELEM_TYPE

-Homologate SHA function names across C and tiny_C library

**TINY_C**
-Modify Rainbow functions to use macro for if

-For getters and setters: you need to consider the ratio between the actual size of the type and the user-specified size
    - maybe use unions and bitfields?

-Including gf16.c from gf256.c could raise a non-definition issue (if a GF16_LOOKUP macro is defined)