from gf16 import *

#R16 = PolynomialRing(gf16, "Z")
#gf256 = gf16.extension("c", modulus=Z^2 + Z + a*b)

c = polygen(gf16)
gf256 = gf16.extension(c**2 + c + a*b, "c")

__gf256_map = list(x for _, x in enumerate(gf256))
__reverse_gf256_map = dict(reverse(enumerate(gf256)))

def int_to_gf256(n):
    return __gf256_map[n]

def gf256_to_int(x):
    return __reverse_gf256_map[x]