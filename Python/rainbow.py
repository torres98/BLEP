from hashlib import sha256, sha384, sha512
from math import log2
import numpy as np
from sys import argv

from effver import offVer
from gf16 import gf16
from gf256 import gf256
from progver import progVer
from utils import fill_matrix_randomly
from ver import Ver

def rainbow_classic_generator(version):

    if version == 1:
        v1, o1, o2 = 36, 32, 32
        element_hex_size = 1
        DIGEST_SIZE = 32

        hash_function = sha256
        gf = gf16
        
    elif version == 2:
        v1, o1, o2 = 68, 32, 48
        element_hex_size = 2
        DIGEST_SIZE = 48

        hash_function = sha384
        gf = gf256

    elif version == 3:
        v1, o1, o2 = 96, 36, 64
        element_hex_size = 2
        DIGEST_SIZE = 64

        hash_function = sha512
        gf = gf256
        
    else:
        raise ValueError("Wrong version number")

    n_variables = v1 + o1 + o2
    n_polynomials = n_variables - v1
    N = (n_variables**2 + n_variables) // 2

    remaining_bytes = n_polynomials - DIGEST_SIZE * (2 // element_hex_size)


    class RainbowClassic:
        
        def __init__(self, pk_path):
            """Setup the public key informations"""

            f = open(pk_path, 'r')
            pk_str = f.read()
            f.close()

            pk_str_offset = pk_str.index('=') + 2

            self.M = np.empty((n_polynomials, N + n_polynomials), dtype=object)
            
            for i in range(N):
                for j in range(n_polynomials):
                    elem_str = pk_str[pk_str_offset:pk_str_offset+element_hex_size]
                    self.M[j, i] = gf(int(elem_str, 16))    
                    pk_str_offset += element_hex_size

            #setup public key matrix for Mv verification
            for i in range(n_polynomials):
                for j in range(N, N + n_polynomials):
                    self.M[i, j] = gf(0)

                self.M[i, N + i] = gf(1)

        @staticmethod
        def get_signature_vector(signature_path, message_path):

            def parse_signature(signature_path):
                f = open(signature_path, 'r')
                sign_str = f.read().rstrip()
                f.close()

                z_start = sign_str.index('=') + 2
                salt_start = z_start + n_variables * element_hex_size

                # extract the map input z (variable values)
                z = [gf(int(sign_str[i:i+element_hex_size], 16)) for i in range(z_start, salt_start, element_hex_size)]
                
                if element_hex_size == 1:
                    for i in range(0, n_variables, 2):
                        z[i+1], z[i] = z[i], z[i+1]

                # compute the quadratic vector s
                s = np.array([z[i] * z[j] for i in range(n_variables) for j in range(i, n_variables)], dtype=object)

                # extract the salt value
                salt = bytes.fromhex(sign_str[salt_start:])

                return s, salt

            def compute_digest(filename, salt):
                f = open(filename, 'rb')
                message = f.read()
                f.close()

                message_digest = hash_function(message).digest()

                message_and_salt_digest = hash_function(message_digest + salt).digest()

                if remaining_bytes == 0:
                    final_digest = message_and_salt_digest.hex()
                else: #fill the remaining bytes with another hash
                    final_digest = (message_and_salt_digest + hash_function(message_and_salt_digest).digest()[:remaining_bytes]).hex()

                return np.array([gf(int(final_digest[i:i+element_hex_size], 16)) for i in range(0, len(final_digest), element_hex_size)], dtype=object)

            #extract signature information
            s, salt = parse_signature(signature_path)
            h = compute_digest(message_path, salt)

            return np.concatenate((s, h), dtype=object)

    return RainbowClassic

RainbowI = rainbow_classic_generator(1)
RainbowIII = rainbow_classic_generator(2)
RainbowV = rainbow_classic_generator(3)

if __name__ == '__main__':

    if len(argv) < 2 or argv[1] not in {'1', '2', '3'}:
        print('Defaulting to RainbowI.')
        Rainbow = RainbowI
        q = 16
    elif argv[1] == '1':
        Rainbow = RainbowI
        q = 16
    elif argv[1] == '2':
        Rainbow = RainbowIII
        q = 256
    elif argv[1] == '3':
        Rainbow = RainbowV
        q = 256

    R_instance = Rainbow('/home/torres/Desktop/Thesis/verification_implementation/tmp/pk.txt')

    v = Rainbow.get_signature_vector(
        '/home/torres/Desktop/Thesis/verification_implementation/tmp/signature.txt', 
        '/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb'
    )

    print(f'VERIFICATION: {Ver(R_instance.M, v)}')

    # for efficient verification
    k = 2 #number of rows for the short verification key
    Z = offVer(R_instance.M, k, q, elem_type = gf16)

    # for progressive verification
    t = 1

    v_guessed = np.empty(R_instance.M.shape[1], dtype=object)

    eff_error_count = prog_error_count = effprog_error_count = 0
    SAMPLE_SIZE = 100

    for _ in range(SAMPLE_SIZE):
        fill_matrix_randomly(v_guessed, min_value = 0, max_value = q, elem_type = gf16)

        standard_ver_result = None

        if Ver(Z, v_guessed):
            if standard_ver_result is None:
                standard_ver_result = Ver(R_instance.M, v_guessed)

            if not standard_ver_result:
                eff_error_count += 1
        
        if progVer(R_instance.M, v_guessed, t):
            if standard_ver_result is None:
                standard_ver_result = Ver(R_instance.M, v_guessed)

            if not standard_ver_result:
                prog_error_count += 1

        if progVer(Z, v_guessed, t):
            if standard_ver_result is None:
                standard_ver_result = Ver(R_instance.M, v_guessed)

            if not standard_ver_result:
                effprog_error_count += 1

    print('EFFICIENT VERIFICATION STATS')
    print(f'Error percentage: {eff_error_count / SAMPLE_SIZE * 100}%')
    print(f'Security bits: {0 if eff_error_count == 0 else log2(eff_error_count / SAMPLE_SIZE)}\n')

    print('PROGRESSIVE VERIFICATION STATS')
    print(f'Error percentage: {prog_error_count / SAMPLE_SIZE * 100}%')
    print(f'Security bits: {0 if prog_error_count == 0 else log2(prog_error_count / SAMPLE_SIZE)}\n')

    print('EFFICIENT+PROGRESSIVE VERIFICATION STATS')
    print(f'Error percentage: {effprog_error_count / SAMPLE_SIZE * 100}%')
    print(f'Security bits: {0 if effprog_error_count == 0 else log2(effprog_error_count / SAMPLE_SIZE)}')
