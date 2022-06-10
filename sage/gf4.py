import sage.all
#from sage.rings.polynomial.polynomial_ring_constructor import PolynomialRing
from sage.rings.polynomial.polynomial_ring import polygen

from gf2 import *

#R2 = PolynomialRing(gf2, "X")
#gf4 = GF(4, "a", modulus=X^2 + X + 1)

a = polygen(gf2)
gf4 = gf2.extension(a**2 + a + 1, "a")

__gf4_map = list(x for _, x in enumerate(gf4))
__reverse_gf4_map = dict(reverse(enumerate(gf4)))

def int_to_gf4(n):
    return __gf4_map[n]

def gf4_to_int(x):
    return __reverse_gf4_map[x]