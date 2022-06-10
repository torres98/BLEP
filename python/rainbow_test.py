from math import log2
import numpy as np
from sys import argv

from ver.effver import generate_random_linear_transformation, offVer
from ver.progver import progVer
from ver.ver import Ver
from ver.utils import fill_matrix_randomly

if len(argv) < 2 or argv[1] not in {'1', '2', '3'}:
    print('Defaulting to RainbowI.')

    from schemes.rainbow import RainbowI as Rainbow
    from schemes.gf.gf16 import gf16 as gf

    q = 16
elif argv[1] == '1':
    from schemes.rainbow import RainbowI as Rainbow
    from schemes.gf.gf16 import gf16 as gf

    q = 16
elif argv[1] == '2':
    from schemes.rainbow import RainbowIII as Rainbow
    from schemes.gf.gf256 import gf256 as gf

    q = 256
elif argv[1] == '3':
    from schemes.rainbow import RainbowV as Rainbow
    from schemes.gf.gf256 import gf256 as gf

    q = 256

pk_path = '/home/torres/Desktop/Thesis/verification_implementation/tmp/pk.txt'
signature_path = '/home/torres/Desktop/Thesis/verification_implementation/tmp/signature.txt'
message_path = '/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb'

PK = Rainbow.parse_public_key(pk_path)
s, salt = Rainbow.parse_signature(signature_path)
message_digest = Rainbow.message_digest(message_path, salt)
u = Rainbow.get_result_vector(message_digest)

print(f'VERIFICATION: {Ver(PK, s, u)}')

# for efficient verification
k = 1 #number of rows for the short verification key
C = generate_random_linear_transformation(k, PK.shape[0], 0, q-1, gf)
svk = C.dot(PK)
u_eff = C.dot(u)

assert(Ver(svk, s, u_eff))
print('Efficiently verified the Rainbow signature.')


# for progressive verification
t = 1

assert(progVer(PK, s, u, t))
print('Progressively verified the Rainbow signature.')


# for efficient and progressive verification
assert(progVer(svk, s, u_eff, t))
print('Efficiently and progressively verified the Rainbow signature.')


signature_guess = np.empty(PK.shape[1], dtype=object)

SAMPLE_SIZE = 100
eff_error_count = prog_error_count = effprog_error_count = 0

for _ in range(SAMPLE_SIZE):
    fill_matrix_randomly(signature_guess, min_value = 0, max_value = q-1, elem_type = gf)

    standard_ver_result = None

    if Ver(svk, signature_guess, u_eff):
        if standard_ver_result is None:
            standard_ver_result = Ver(PK, signature_guess, u)

        if not standard_ver_result:
            eff_error_count += 1
    
    if progVer(PK, signature_guess, u, t):
        if standard_ver_result is None:
            standard_ver_result = Ver(PK, signature_guess, u)

        if not standard_ver_result:
            prog_error_count += 1

    if progVer(svk, signature_guess, u_eff, t):
        if standard_ver_result is None:
            standard_ver_result = Ver(PK, signature_guess, u)

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
