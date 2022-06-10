from effver import *
from progver import *

if __name__ == '__main__':

    M = generate_random_public_key()

    #for efficient verification
    k = 2
    Z = offVer(M, k)

    #for progressive verification
    t = 2

    eff_error_count = prog_error_count = progeff_error_count = 0

    print(f'''
        COMPLETE VERIFICATION TEST

        EFFICIENT: Using a {k}x{COLUMNS} matrix instead of the {ROWS}x{COLUMNS} one
        PROGRESSIVE: Testing {t} out of {ROWS} steps
        EFFICIENT + PROGRESSIVE: Using a {k}x{COLUMNS} and testing {t} out of {k} steps
        (q = {q} with {SAMPLE_SIZE} tests)
    ''')

    for _ in range(SAMPLE_SIZE):
        #do not create null vector
        v = np.random.randint(0, q, (COLUMNS, 1))

        verification_output = None
            
        # efficient
        if onVer(Z, v):
            if verification_output is None:
                verification_output = Ver(M, v)

            if not verification_output:
                eff_error_count += 1

        # progressive
        if progVerRand(M, v, t):
            if verification_output is None:
                verification_output = Ver(M, v)

            if not verification_output:
                prog_error_count += 1

        # progressive + efficient
        """if progVerRand(Z, v, t):
            if verification_output is None:
                verification_output = Ver(M, v)

            if not verification_output:
                progeff_error_count += 1"""

    print(f'EFFICIENT VERIFICATION')
    print(f'Error percentage: {eff_error_count / SAMPLE_SIZE * 100}%')
    print(f'{0 if eff_error_count == 0 else log2(eff_error_count / SAMPLE_SIZE)}\n')

    print(f'PROGRESSIVE VERIFICATION')
    print(f'Error percentage: {prog_error_count / SAMPLE_SIZE * 100}%')
    print(f'{0 if prog_error_count == 0 else log2(prog_error_count / SAMPLE_SIZE)}\n')

    print(f'EFFICIENT + PROGRESSIVE VERIFICATION')
    print(f'Error percentage: {progeff_error_count / SAMPLE_SIZE * 100}%')
    print(f'{0 if progeff_error_count == 0 else log2(progeff_error_count / SAMPLE_SIZE)}')