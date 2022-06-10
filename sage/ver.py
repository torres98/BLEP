from sage.all import *
from math import log2

ROWS = 100
COLUMNS = 150

q = 1021 #must be prime
RING = Integers(q)

SAMPLE_SIZE = 10000000

def generate_random_public_key():
    """Generates at random a valid public key"""
    
    def check_signature_existance(M):
        """Checks if the given matrix admits a non-trivial signature"""
        for homogeneous_soln in M.right_kernel().basis():
            if any(homogeneous_soln):
                return True

        return False

    M = random_matrix(RING, ROWS, COLUMNS)

    while not check_signature_existance(M):
        M = random_matrix(RING, ROWS, COLUMNS)

    return M
    
def Ver(M, v):
    return not any(M * v)