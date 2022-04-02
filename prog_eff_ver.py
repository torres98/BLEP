from ver import *
from effver import offVer
from progver import progVer, progVerRand

if __name__ == '__main__':

    #for now just create M randomly
    M = generate_random_public_key()

    #for efficient verification
    k = ROWS // 5
    Z = offVer(M, k)

    #for progressive verification
    t = min(2, k)

    error_count = 0

    print(f'''
        EFFICIENT AND PROGRESSIVE VERIFICATION TEST
        Using a {k}x{COLUMNS} matrix instead of the {ROWS}x{COLUMNS} one
        Testing {t} out of {k} steps
        (q = {q} with {SAMPLE_SIZE} tests)
    ''')

    # Since almost always the signature created are invalid, the error rate is
    # a bit skewed towards greater values

    # But maybe i should just consider those signatures... 

    for _ in range(SAMPLE_SIZE):
        v = np.random.randint(0, q, (COLUMNS, 1))

        if progVerRand(Z, v, t) and not Ver(M, v):
            #print("ERROR FOUND")
            #print(f'svk\n{Z}')
            #print(f'signature\n{v.transpose()}')
            error_count += 1

    print(f'Error percentage: {error_count / SAMPLE_SIZE * 100}%')
    print(f'{0 if error_count == 0 else log2(error_count / SAMPLE_SIZE)}')