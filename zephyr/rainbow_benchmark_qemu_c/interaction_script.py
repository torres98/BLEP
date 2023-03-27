import os
import signal
import subprocess

from effprog.ver.effver import generate_random_linear_transformation
from effprog.uart_utils import *
from effprog.parsing_utils import parse_args


shell_args = parse_args(True)

RAINBOW_VERSION = shell_args['RAINBOW_VERSION']
SAMPLE_SIZE = shell_args['SAMPLE_SIZE']

if RAINBOW_VERSION == 1:
    from effprog.schemes.rainbow import RainbowI as Rainbow
    from effprog.schemes.gf.gf16 import gf16 as gf

    q = 16

elif RAINBOW_VERSION == 2:
    from effprog.schemes.rainbow import RainbowIII as Rainbow
    from effprog.schemes.gf.gf256 import gf256 as gf

    q = 256

else:
    from effprog.schemes.rainbow import RainbowV as Rainbow
    from effprog.schemes.gf.gf256 import gf256 as gf

    q = 256

PK_PATH = f'/home/torres/Desktop/Thesis/verification_implementation/tmp/pk{shell_args["RAINBOW_VERSION"]}.txt'

#BUILD APPLICATION
#if rebuild has been requested or there is no build directory...
if not shell_args['SKIP_BUILD']:
    #Generate random transformation and corresponding svk
    print('Creating SVK...', end=' ', flush=True)

    PK = Rainbow.parse_public_key(PK_PATH)
    C = generate_random_linear_transformation(shell_args['SVK_NROWS'], Rainbow.n_polynomials, 0, q-1, gf)
    SVK = C.dot(PK)

    f = open('src/svk.h', 'w')
    f.write(f'#include <stdint.h>\n\n#define SVK_NROWS {shell_args["SVK_NROWS"]}\n\nconst uint8_t short_private_key[{SVK.size}] = {{')

    for i in range(SVK.shape[0]):
        for j in range(SVK.shape[1]):
            f.write(f'{SVK[i, j]}, ')

    f.write(f'}};\n\nconst uint8_t private_transformation[{C.size}] = {{')

    for i in range(C.shape[0]):
        for j in range(C.shape[1]):
            f.write(f'{C[i, j]}, ')

    f.write('};\n')
    f.close()

    print('DONE')

    print('Building project...', end=' ', flush=True)

    gf_lookup_level = {
        '0': 0,
        '1': 1,
        '1r': 2,
        '2': 3,
        '2r': 4
    }

    build_cmd = [
        'west',
        'build',
        '-p',
        f'-b={shell_args["BOARD"]}',
        '--',
        f'-DRAINBOW_VERSION={RAINBOW_VERSION}',
        f'-DGF{q}_LOOKUP={gf_lookup_level[shell_args["LOOKUP_LEVEL"]]}',
        f'-DSAMPLE_SIZE={SAMPLE_SIZE}',
        f'-DPROGRESSIVE_STEPS={shell_args["PROGRESSIVE_STEPS"]}'
    ]

    build_process = subprocess.Popen(build_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    build_output = build_process.communicate()

    if build_process.returncode != 0:
        print(build_output[0].decode('utf-8'))
        print(build_output[1].decode('utf-8'))
        exit(-1)

    print('DONE')
    print(f'Memory region{build_output[0].decode("utf-8").split("Memory region")[1]}')

#LAUNCH APPLICATION
with subprocess.Popen(shell_args['FLASH_CMD'], stdout=subprocess.PIPE, preexec_fn=os.setsid) as proc:
    proc_stdout = proc.stdout
    
    output_line = proc_stdout.readline()

    while(not output_line.startswith(b'*** Booting')):
        output_line = proc_stdout.readline()

    for _ in range(3):
        print(proc_stdout.readline().decode('utf-8'), end='')

    os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
