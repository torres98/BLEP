import numpy as np
from random import randint

def fill_matrix_randomly(M, min_value, max_value, elem_type):
    M_2d = np.atleast_2d(M)
    n, m = M_2d.shape

    for i in range(n):
        for j in range(m):
            M_2d[i, j] = elem_type(randint(min_value, max_value))
