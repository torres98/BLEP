import numpy as np

def progVer(M, v, t, rand = True):
    def progStep(i, M, v, row_perm):
        return not bool(M[row_perm[i]].dot(v))

    if t == 0:
        return False

    if rand:
        row_perm = np.random.choice(len(M), size=t, replace=False)
    else:
        row_perm = np.arange(t)

    for i in range(t):
        if not progStep(i, M, v, row_perm):
            return False

    return True