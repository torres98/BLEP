from ver import *
from random import randint

def progVer(M, v, t):
    def progStep(i, M, v):
        return (M[i-1].dot(v) % q) == 0
        
    if t == 0:
        return False

    for i in range(1, t+1):
        if not progStep(i, M, v):
            return False

    return True

def progVerRand(M, v, t):
    def progStep(i, M, v, row_perm):
        return (M[row_perm[i-1]].dot(v) % q) == 0

    if t == 0:
        return False

    row_perm = np.random.choice(len(M), size=t, replace=False)

    for i in range(1, t+1):
        if not progStep(i, M, v, row_perm):
            return False

    return True

if __name__ == '__main__':

    #for now just create M randomly
    M = np.random.randint(0, q, (ROWS, COLUMNS))

    error_count = 0

    t = 2 #randint(0, ROWS//4)
    print(f'''
        PROGRESSIVE VERIFICATION TEST
        Testing {t} out of {ROWS} steps
        (q = {q} with {SAMPLE_SIZE} tests)
    ''')

    for _ in range(SAMPLE_SIZE):
        v = np.random.randint(0, q, (COLUMNS, 1))

        if progVerRand(M, v, t) and not Ver(M, v):
            #print("ERROR FOUND")
            #print(f'svk\n{Z}')
            #print(f'signature\n{v.transpose()}')
            error_count += 1

    print(f'Error percentage: {error_count / SAMPLE_SIZE * 100}%')
    print(f'Power: {0 if error_count == 0 else log2(error_count / SAMPLE_SIZE)}')