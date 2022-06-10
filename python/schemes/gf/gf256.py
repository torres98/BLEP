from .gf16 import gf16_add, gf16_mul, gf16_mul_8

def gf256_add(a, b):
    return gf16_add(a >> 4, b >> 4) << 4 | gf16_add(a & 0xf, b & 0xf)

def gf256_mul(a, b):
    a0 = a & 15
    a1 = (a >> 4)
    b0 = b & 15
    b1 = (b >> 4)
    a0b0 = gf16_mul(a0, b0)
    a1b1 = gf16_mul(a1, b1)
    a0b1_a1b0 = gf16_mul(a0 ^ a1, b0 ^ b1) ^ a0b0 ^ a1b1
    a1b1_x8 = gf16_mul_8(a1b1)
    return ((a0b1_a1b0 ^ a1b1) << 4) ^ a0b0 ^ a1b1_x8

add_lookup_table = [[gf256_add(i, j) for j in range(256)] for i in range(256)]
mul_lookup_table = [[gf256_mul(i, j) for j in range(256)] for i in range(256)]

def gf256_add(a, b):
    return add_lookup_table[a][b]

def gf256_mul(a, b):
    return mul_lookup_table[a][b]

class gf256:
    def __init__(self, v = 0):
        self.v = v % 256

    def __add__(self, a):
        if isinstance(a, gf256):
            return gf256(gf256_add(self.v, a.v))
            
        return gf256(gf256_add(self.v, a % 256))

    def __radd__(self, a):
        return self + a

    def __mul__(self, a):
        if isinstance(a, gf256):
            return gf256(gf256_mul(self.v, a.v))

        return gf256(gf256_mul(self.v, a % 256))

    def __rmul__(self, a):
        return self * a

    def __mod__(self, q):
        return gf256(self.v % q)

    def __bool__(self):
        return self.v != 0

    def __eq__(self, __o):
        return self.v == __o.v
        
    def __str__(self):
        return str(self.v)

    def __index__(self):
        return self.v
