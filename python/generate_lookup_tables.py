from schemes.gf.gf256 import gf256 as gf

q = 256

print(f'unsigned char gf{q}_sub_lookup[{q}][{q}] = {{')

for a in range(q):
    print('    {', end='')

    for b in range(q):

        #find c such that a = b + c
        for c in range(q):
            if gf(c) + gf(b) == gf(a):
                print(f'{c}, ', end = '')
                break

    print('},')

print('};')

print(f'unsigned char gf{q}_div_lookup[{q}][{q}] = {{')

for a in range(q):
    print('    {', end='')

    for b in range(q):

        #find c such that a = b * c
        for c in range(q):
            if gf(c) * gf(b) == gf(a):
                print(f'{c}, ', end = '')
                break

    print('},')

print('};')

print(f'unsigned char gf{q}_inv_lookup[{q-1}] = {{', end = '')

for a in range(1, q):

    # find b such that a*b = 1
    for b in range(q):

        if gf(a) * gf(b) == gf(1):
            print(f'{b}, ', end = '')
            break

print('};')