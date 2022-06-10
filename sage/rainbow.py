from sys import argv
from hashlib import sha256, sha384, sha512

from gf256 import *
from effver import *

def rainbow_classic_generator(version):

    if version == 1:
        element_size = 1
        hash_function = sha256
        field = gf16
        gf = int_to_gf16

        v1, o1, o2 = 36, 32, 32

    elif version == 2:
        element_size = 2
        hash_function = sha384
        field = gf256
        gf = int_to_gf256

        v1, o1, o2 = 68, 32, 48

    elif version == 3:
        element_size = 2
        hash_function = sha512
        field = gf256
        gf = int_to_gf256

        v1, o1, o2 = 96, 36, 64
    else:
        raise ValueError("Wrong version number")

    n_variables = v1+o1+o2
    n_polynomials = n_variables - v1
    N = (n_variables**2 + n_variables) // 2

    class RainbowClassic:
        
        def __init__(self, pk_path):
            """Setup the public key informations"""

            f = open(pk_path, 'r')
            pk_str = f.read()
            f.close()

            pk_str_offset = pk_str.index('=') + 2

            self.M = Matrix(field, n_polynomials, N + n_polynomials)
            
            for i in range(n_polynomials):
                for j in range(N):
                    elem_str = pk_str[pk_str_offset:pk_str_offset+element_size]
                    self.M[i, j] = gf(int(elem_str, 16))    
                    pk_str_offset += element_size

            #setup public key matrix for Mv verification
            for i in range(n_polynomials):
                self.M[i, N + i] = gf(1)

        @staticmethod
        def get_signature_vector(signature_path, message_path):

            def parse_signature(signature_path):
                f = open(signature_path, 'r')
                sign_str = f.read().rstrip()
                f.close()

                z_start = sign_str.index('=') + 2
                salt_start = z_start + n_variables * element_size

                # extract the map input z (variable values)
                z = [gf(int(sign_str[i:i+element_size], 16)) for i in range(z_start, salt_start, element_size)]
                
                if element_size == 1:
                    for i in range(0, n_variables, 2):
                        z[i+1], z[i] = z[i], z[i+1]

                for x in z:
                    print(f'{gf16_to_int(x)},', end=" ")

                print()

                # compute the quadratic vector s
                s = vector(field, [z[i] * z[j] for i in range(n_variables) for j in range(i, n_variables)])

                # extract the salt value
                salt = bytes.fromhex(sign_str[salt_start:])

                return s, salt

            def compute_digest(filename, salt):
                f = open(filename, 'rb')
                message = f.read()
                f.close()

                message_digest = hash_function(message).digest()
                message_salt_digest = hash_function(message_digest + salt).digest()

                if len(message_salt_digest) * (2 // element_size) == n_polynomials:
                    final_digest = message_salt_digest.hex()
                else: #fill the remaining bytes with another hash
                    remaining_bytes = n_polynomials - len(message_salt_digest) #could be wrong for...
                    final_digest = (message_salt_digest + hash_function(message_salt_digest).digest()[:remaining_bytes]).hex()

                print(final_digest)
                return vector(field, [gf(int(final_digest[i:i+element_size], 16)) for i in range(0, len(final_digest), element_size)])

            #extract signature information
            s, salt = parse_signature(signature_path)
            h = compute_digest(message_path, salt)

            return vector(field, s.list() + h.list()) #Matrix(s.M + h.M)

        def verify(self, v):
            return not any(self.M * v)

    return RainbowClassic

RainbowI = rainbow_classic_generator(1)
RainbowIII = rainbow_classic_generator(2)
RainbowV = rainbow_classic_generator(3)

if __name__ == '__main__':

    if len(argv) < 2 or argv[1] not in {'1', '2', '3'}:
        print('Defaulting to RainbowI.')
        Rainbow = RainbowI
    elif argv[1] == '1':
        Rainbow = RainbowI
    elif argv[1] == '2':
        Rainbow = RainbowIII
    elif argv[1] == '3':
        Rainbow = RainbowV

    R_instance = Rainbow('/home/torres/Desktop/Thesis/verification_implementation/tmp/pk.txt')

    v = Rainbow.get_signature_vector(
        '/home/torres/Desktop/Thesis/verification_implementation/tmp/signature.txt', 
        '/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb'
    )

    import time
    t0 = time.time()
    print(f'VERIFICATION: {R_instance.verify(v)}')
    print(time.time() - t0)

    """Z = offVer(R_instance.M, 1)

    error_count = 0
    SAMPLE_SIZE = 1000

    for _ in range(SAMPLE_SIZE):
        v_guessed = np.random.randint(0, q, (R_instance.M.shape[1], 1))

        if onVer(Z, v_guessed) and not R_instance.verify(v_guessed):
            error_count += 1

    print(f'Error percentage: {error_count / SAMPLE_SIZE * 100}%')
    print(f'{0 if error_count == 0 else log2(error_count / SAMPLE_SIZE)}')"""