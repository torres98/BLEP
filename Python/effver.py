import numpy as np

from utils import fill_matrix_randomly

def offVer(M, k, max_value, elem_type=int, return_transformation=False):
    n = M.shape[0]

    #Create the random matrix C
    C = np.empty((k, n), dtype=elem_type)

    fill_matrix_randomly(C, 0, max_value, elem_type)

    #probably can't be done easily
    #while not np.linalg.matrix_rank(C) == k:
    #    C = np.random.randint(0, q, (k, n))

    if return_transformation:
        return C, C.dot(M)
    
    return C.dot(M)

def are_parallel(v, w):
    #extract couples different than 0,0
    def solve_linear_congruence(a, b, q):
        """ Describe all solutions to ax = b  (mod m), or raise ValueError. """
        x = var('x')
        #print(a,b)
        return solve_mod(a*x == b, q)[0][0] if a != 0 else solve_mod(b*x == a, q)[0][0]

    #there can be only one solution!
    for i in range(COLUMNS):
        a = int(v[i][0])
        b = int(w[i][0])

        if a != 0 or b != 0:
            coeff = solve_linear_congruence(a, b, q)
            break

    return np.array_equal(coeff * v % q, w)

def attack(M, Z):
    #improve how you generate the next signature
    def generate_valid_signature_guess(forbidden_w_vectors):
        v = np.random.randint(0, q, (COLUMNS, 1))
        w = M.dot(v) % q

        while any(map(lambda x: are_parallel(w, x), forbidden_w_vectors)):
            v = np.random.randint(0, q, (COLUMNS, 1))
            w = M.dot(v) % q

        return v, w

    forbidden_w_vectors = []

    # you're just changing the signature, but not the message!
    v, w = generate_valid_signature_guess(forbidden_w_vectors)

    onVer_output = onVer(Z, v)

    while not (onVer_output and np.any(w)) or not np.any(v):

        if not onVer_output: #Leakage
            forbidden_w_vectors.append(w)
            print(f'Number of leakages: {len(forbidden_w_vectors)}')

        v, w = generate_valid_signature_guess(forbidden_w_vectors)

        onVer_output = onVer(Z, v)

    print(f'Number of leakages: {len(forbidden_w_vectors)}')
    return v

if __name__ == '__main__':

    M = generate_random_public_key()

    k = 2
    Z = offVer(M, k)

    """
    forged_signature = attack(M, Z)

    print(f'''
        Forged Signature: {[x[0] for x in forged_signature]}
        (normal verification {Ver(M, forged_signature)}, efficient verification {onVer(Z, forged_signature)}))
    ''')

    """
    error_count = 0

    print(f'''
        EFFICIENT VERIFICATION TEST
        Using a {k}x{COLUMNS} matrix instead of the {ROWS}x{COLUMNS} one
        (q = {q} with {SAMPLE_SIZE} tests)
    ''')

    for _ in range(SAMPLE_SIZE):
        v = np.random.randint(0, q, (COLUMNS, 1))

        if onVer(Z, v) and not Ver(M, v):
            error_count += 1

    print(f'Error percentage: {error_count / SAMPLE_SIZE * 100}%')
    print(f'{0 if error_count == 0 else log2(error_count / SAMPLE_SIZE)}')