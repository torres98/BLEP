from .gf2 import gf2_add

def gf4_add(a, b):
    return gf2_add(a >> 1, b >> 1) << 1 | gf2_add(a & 0x1, b & 0x1)

def gf4_mul_2(a):
    r = a << 1
    r ^= (a >> 1) * 7
    return r

def gf4_mul_3(a):
    msk = (a - 2) >> 1
    return (msk & (a * 3)) | ((~msk) & (a - 1))

def gf4_mul(a, b):
    r = a * (b & 1)
    return r ^ (gf4_mul_2(a) * (b >> 1))
