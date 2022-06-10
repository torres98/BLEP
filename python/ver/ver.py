import numpy as np

def Ver(PK, s, u = None):
    if u is None:
        return not np.any(PK.dot(s))

    return np.array_equal(PK.dot(s), u)

"""
def Ver(PK, s, u = None):
    print("Yolo")
    n, m = PK.shape

    if s.shape[0] != m:
        raise ValueError

    elem_type = type(PK[0,0])

    if u is None:
        for i in range(n):
            cell = elem_type()

            for j in range(m):
                cell += PK[i,j] * s[j]

            if bool(cell):
                return False
    else:
        for i in range(n):
            cell = elem_type()

            for j in range(m):
                cell += PK[i,j] * s[j]

            if cell != u[i]:
                return False

    return True
"""

def generate_random_public_key():
    """Generates at random a valid public key"""
    
    def check_signature_existance(M):
        """Checks if the given matrix admits a non-trivial signature"""

        M_matrix = Matrix(GF(q), M)
        M_matrix.solve_right(vector(GF(q), [0] * ROWS))

        for homogeneous_soln in M_matrix.right_kernel().basis():
            if any(homogeneous_soln):
                return True

        return False

    M = np.random.randint(0, q, (ROWS, COLUMNS))

    while not check_signature_existance(M):
        M = np.random.randint(0, q, (ROWS, COLUMNS))

    return M
    