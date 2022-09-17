from schemes.gf.gf256 import gf256 as gf

q = 256
spaces = '    ' if q == 16 else '     '
pad = 2 if q == 16 else 3

print(f'const unsigned char gf{q}_add_lookup[] = {{')

for a in range(q):
    print(f'   {spaces * a}', end = '')
    for b in range(a+1, q):
        print(f'{str(gf(a) + gf(b)): >{pad}}', end = ', ')
    print()
print('};')

print(f'const unsigned char gf{q}_mul_lookup[] = {{')

for a in range(1, q):
    print(f'   {spaces * (a-1)}', end = '')
    for b in range(a, q):
        print(f'{str(gf(a) * gf(b)): >{pad}}', end = ', ')
    print()
print('};')

exit()

print(f'const unsigned char gf{q}_mul_lookup[{q}][{q}] = {{')

for a in range(q):
    print('   ', end = '')
    for b in range(a+1, q):
        print(gf(a) * gf(b), end = ', ')
    print()

print('};')

print(f'const unsigned char gf{q}_div_lookup[{q}][{q}] = {{')

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

print(f'const unsigned char gf{q}_inv_lookup[{q-1}] = {{', end = '')

for a in range(1, q):

    # find b such that a*b = 1
    for b in range(q):

        if gf(a) * gf(b) == gf(1):
            print(f'{b}, ', end = '')
            break

print('};')