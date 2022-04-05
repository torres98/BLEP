import numpy as np
from sage.all import *
from math import log2

ROWS = 30
COLUMNS = 50
q = 1021 #must be prime

SAMPLE_SIZE = 10000000

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
    
def Ver(M, v):
    return not np.any(M.dot(v) % q)