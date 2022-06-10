import numpy as np

def progVer(PK, s, u = None, t = 0, rand = True):
    def progStep(i, PK, s, u, row_perm):
        if u is None:
            return not bool(PK[row_perm[i]].dot(s))
        
        return PK[row_perm[i]].dot(s) == u[row_perm[i]]

    if t == 0:
        return False

    if rand:
        row_perm = np.random.choice(PK.shape[0], size=t, replace=False)
    else:
        row_perm = np.arange(t)

    for i in range(t):
        if not progStep(i, PK, s, u, row_perm):
            return False

    return True
