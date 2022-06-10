from ver import *
from gf16 import *

def offVer(M, k, return_transformation=False):
    n = M.nrows()

    #Create the random matrix C
    C = random_matrix(RING, k, n)

    while C.rank() != k:
        C = random_matrix(RING, k, n)
    
    if return_transformation:
        return C, C * M
    
    return C * M

def onVer(Z, v):
    """
    - Z: short verification key
    - v: signature of the message
    """
    return not any(Z * v)