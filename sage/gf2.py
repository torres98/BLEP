from sage.rings.finite_rings.finite_field_constructor import GF

gf2 = GF(2)

def reverse(l):
    for x, y in l:
        yield y, x