import numpy as np
from gf4 import gf4_add, gf4_mul, gf4_mul_3, gf4_mul_2

"works with tower representation"

def gf16_add(a, b):
    return gf4_add(a >> 2, b >> 2) << 2 | gf4_add(a & 0x3, b & 0x3)

def gf16_mul_8(a):
    a0 = a & 3
    a1 = a >> 2
    return (gf4_mul_2(a0 ^ a1) << 2) | gf4_mul_3(a1)

def gf16_mul(a, b):
    a0 = a & 3
    a1 = (a >> 2)
    b0 = b & 3
    b1 = (b >> 2)
    a0b0 = gf4_mul(a0, b0)
    a1b1 = gf4_mul(a1, b1)
    a0b1_a1b0 = gf4_mul(a0 ^ a1, b0 ^ b1) ^ a0b0 ^ a1b1
    a1b1_x2 = gf4_mul_2(a1b1)
    return ((a0b1_a1b0 ^ a1b1) << 2) ^ a0b0 ^ a1b1_x2

# Use memory because too slow otherwise

add_lookup_table = [[gf16_add(i, j) for j in range(16)] for i in range(16)]
mul_lookup_table = [[gf16_mul(i, j) for j in range(16)] for i in range(16)]

def gf16_add(a, b):
    return add_lookup_table[a][b]

def gf16_mul(a, b):
    return mul_lookup_table[a][b]

"""def gf16_dot(A, B):
    n = len(A)
    m = 1 if len(B.shape) == 1 else B.shape[1]

    C = np.zeros((n, m), dtype=int)

    for i in range(n):
        for j in range(m):
            for h in range(A.shape[1]):
                C[i][j] = gf16_add(C[i][j], gf16_mul(A[i][h], B[h][j]))

    return C"""

class gf16:
    def __init__(self, v):
        self.v = v % 16

    def __add__(self, a):
        if isinstance(a, gf16):
            return gf16(gf16_add(self.v, a.v))
            
        return gf16(gf16_add(self.v, a % 16))

    def __radd__(self, a):
        return self + a

    def __mul__(self, a):
        if isinstance(a, gf16):
            return gf16(gf16_mul(self.v, a.v))

        return gf16(gf16_mul(self.v, a % 16))

    def __rmul__(self, a):
        return self * a

    def __mod__(self, q):
        return gf16(self.v % q)

    def __bool__(self):
        return self.v != 0

    def __str__(self):
        return str(self.v)

    def __index__(self):
        return self.v