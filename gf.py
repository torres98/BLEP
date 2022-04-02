from math import sqrt

def gf_add(a, b, n):
    if n == 2:
        return a ^ b

    new_n = sqrt(n)
    h = sqrt(new_n)
    l = new_n - 1

    return gf_add(a >> h, b >> h, new_n) << h | gf_add(a & l, b & l, new_n)