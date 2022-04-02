class Matrix:
    def __init__(self, M):
        self.M = M #should be copied but whatever...

    @property
    def rows(self):
        return len(self.M)

    @property
    def columns(self):
        return 1 if not isinstance(self.M[0], list) else len(self.M[0])

    def __getitem__(self, key):
        return self.M[key]

    def __setitem__(self, key, value):
        self.M[key] = value

    def __mul__(self, other):
        if self.columns != other.rows:
            raise ValueError(f'Incompatible matrix sizes ({self.columns} with {other.rows}).')

        if other.columns == 1:
            O = Matrix([0]*self.rows)

            for i in range(self.rows):
                for h in range(self.columns):
                    O[i] += self[i][h] * other[h]
        else:
            O = Matrix([[0]*other.columns for _ in range(self.rows)])

            for i in range(self.rows):
                for j in range(other.columns):
                    for h in range(self.columns):
                        O[i][j] += self[i][h] * other[h][j]

        return O

    def any(self):
        if self.columns == 1:
            for x in self.M:
                if x:
                    return True
        else:
            for i in range(self.rows):
                for j in range(self.columns):
                    if self.M[i][j]:
                        return True

        return False
