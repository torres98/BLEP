def append_row(M, row):
    return Matrix(M.rows()[:0]+[row]+M.rows()[0:])

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

def are_columns_linearly_indipendent(A):
    if A.nrows() == A.ncols():
        return A.determinant() != 0

    A_rref = A.echelon_form()

    for i in range(A_rref.ncols()):
        if A_rref[i][i] != 1 or any(A_rref[i][:i]):
            return False

    return True

def in_same_span(w, forbidden_w_vectors):
    Q = append_row(forbidden_w_vectors, w).transpose()
    
    return not are_columns_linearly_indipendent(Q)
    
def forge_signature(M, Z):
    #improve how you generate the next signature
    def generate_valid_signature_guess(forbidden_w_vectors):
        v = random_vector(RING, COLUMNS)
        w = M * v

        # while they are in the same span in_same_span(w, forbidden_w_vectors):
        while in_same_span(w, forbidden_w_vectors):
            #print(forbidden_w_vectors, w, end = '\n\n')
            v = random_vector(RING, COLUMNS)
            w = M * v

        return v, w

    """def generate_valid_signature_guess(forbidden_w_vectors):
        #v = random_vector(RING, COLUMNS)
        w = random_vector(RING, ROWS)

        # while they are in the same span in_same_span(w, forbidden_w_vectors):
        while in_same_span(w, forbidden_w_vectors):
            #print(forbidden_w_vectors, w, end = '\n\n')
            w = random_vector(RING, ROWS)
            print("FAIL")

        return M.solve_right(w), w"""

    forbidden_w_vectors = Matrix(RING, 0, ROWS)

    # you're just changing the signature, but not the message!
    v, w = generate_valid_signature_guess(forbidden_w_vectors)

    onVer_output = onVer(Z, v)

    while not (onVer_output and any(w)) or not any(v):

        if not onVer_output: #Leakage
            forbidden_w_vectors = append_row(forbidden_w_vectors, w)

        v, w = generate_valid_signature_guess(forbidden_w_vectors)

        onVer_output = onVer(Z, v)

    return v

def leak_C_info(C):

    W = random_matrix(RING, ROWS, ROWS)
    
    #each column has to have at least a non zero value
    while not are_columns_linearly_indipendent(W) or not all(any(col) for col in (C * W).columns()):
        if any(not any(col) for col in (C * W).columns()):
            print("FOUND FORGERY")
            return W

        W = random_matrix(RING, ROWS, ROWS)

    return W

def leak_C_info_v2(M, C):

    V = random_matrix(RING, COLUMNS, ROWS)
    W = M*V

    while not are_columns_linearly_indipendent(W) or not all(any(col) for col in (C * W).columns()):
        if any(not any(col) for col in (C * W).columns()):
            print("FOUND FORGERY")
            return W

        V = random_matrix(RING, COLUMNS, ROWS)
        W = M*V
    
    return W

import time

if __name__ == '__main__':

    #Setup
    k = 2
    M = generate_random_public_key()
    C, Z = offVer(M, k, True)

    #Get W matrix
    t0 = time.time()
    W = leak_C_info(C)
    print(time.time() - t0)

    """print(f'{W}\n\nDETERMINANT: {W.det()}\n')
    print('SIGNATURES:')
    for w in W.columns():
        print(M.solve_right(w))"""

    t0 = time.time()
    W = leak_C_info_v2(M, C)
    print(time.time() - t0)

    """print(f'{W}\n\nDETERMINANT: {W.det()}\n')
    print('SIGNATURES:')
    for w in W.columns():
        print(M.solve_right(w))"""

    #Try forging a signature
    forged_signature = forge_signature(M, Z)

    print(f'''Forged Signature: {forged_signature}
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
    """