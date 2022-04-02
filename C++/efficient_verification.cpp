#include "matrix.cpp"

template <typename Element>
Matrix<Element> offVer(Matrix<Element> &M, unsigned int k) {

    Matrix<Element> C = Matrix<Element>(k, M.rows); 
    fill_matrix_randomly(C); 

    /*while not np.linalg.matrix_rank(C) == k:
        C = np.random.randint(0, q, (k, n))*/

    return C * M;
}

template <typename Element>
Element norm(Matrix<Element> &v) {
    Element max;

    for (unsigned int i = 0; i < v.rows; i++)
        if (v[i][0] > max)
            max = v[i][0];
    
    return max;

}

template <typename Element>
bool norm_check(Matrix<Element> &v, Element &beta) {
    for (unsigned int i = 0; i < v.rows; i++)
        if (v[i][0] > beta)
            return false;
    
    return true;

}

/*
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
*/