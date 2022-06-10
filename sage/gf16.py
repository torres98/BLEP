from gf4 import *

#R4 = PolynomialRing(F4, "Y")
#gf16 = gf4.extension("b", modulus=Y^2 + Y + a)

b = polygen(gf4)
gf16 = gf4.extension(b**2 + b + a, "b")

__gf16_map = list(x for _, x in enumerate(gf16))
__reverse_gf16_map = dict(reverse(enumerate(gf16)))

def int_to_gf16(n):
    return __gf16_map[n]

def gf16_to_int(x):
    return __reverse_gf16_map[x]