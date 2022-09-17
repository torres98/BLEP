import secrets
import serial
import subprocess

from effprog.ver.effver import generate_random_linear_transformation
from effprog.uart_utils import *
from effprog.parsing_utils import parse_args

SHA_CHUNK_SIZE = 32678

PK_PATH = '/home/torres/Desktop/Thesis/verification_implementation/tmp/pk.txt'
MSG_PATH = '/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb'

TTY_PORT = 115200

shell_args = parse_args(True)

if shell_args['RAINBOW_VERSION'] == 1:
    from effprog.schemes.rainbow import RainbowI as Rainbow
    from effprog.schemes.gf.gf16 import gf16 as gf

    q = 16

elif shell_args['RAINBOW_VERSION'] == 2:
    from effprog.schemes.rainbow import RainbowIII as Rainbow
    from effprog.schemes.gf.gf256 import gf256 as gf

    q = 256

else:
    from effprog.schemes.rainbow import RainbowV as Rainbow
    from effprog.schemes.gf.gf256 import gf256 as gf

    q = 256


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

#BUILD APPLICATION
#if rebuild has been requested or there is no build directory...
if not shell_args['SKIP_BUILD']:
    print('Building project...', end=' ', flush=True)

    build_cmd = [
        'west',
        'build',
        '-p',
        f'-b={shell_args["BOARD"]}',
        '--',
        f'-DRAINBOW_VERSION={shell_args["RAINBOW_VERSION"]}',
        f'-DGF{q}_LOOKUP={shell_args["LOOKUP_LEVEL"]}'
    ]

    build_process = subprocess.Popen(build_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    build_output = build_process.communicate()

    if build_process.returncode != 0:
        print(build_output[0].decode('utf-8'))
        print(build_output[1].decode('utf-8'))
        exit(-1)

    print('DONE')

#LAUNCH APPLICATION
with serial.Serial(shell_args['TTY_DEVICE'], TTY_PORT) as serial_device:
    subprocess.check_call(shell_args['FLASH_CMD'])
    print()

    uart_readline(serial_device)

    SAMPLE_SIZE = shell_args['SAMPLE_SIZE']

    uart_send_int(serial_device, shell_args['PROGRESSIVE_STEPS'])
    uart_send_int(serial_device, SAMPLE_SIZE)

    #MESSAGE
    with open(MSG_PATH, 'rb') as message_file:
        message = message_file.read()

    mlen = len(message)
    uart_send_int(serial_device, mlen)
    print('Message length correctly sent.')

    #receive gothrough
    uart_wait(serial_device)

    for i in range(0, mlen, SHA_CHUNK_SIZE):
        uart_send_segmented(serial_device, message[i: i + SHA_CHUNK_SIZE])

    print('Message correctly sent.')

    eff_errors = 0
    effprog_errors = 0

    for i in range(SAMPLE_SIZE):
        # generate random signature
        signature = secrets.token_bytes(Rainbow.n_variables // (2 // Rainbow.element_hex_size) + 16)

        #receive gothrough
        uart_wait(serial_device)

        uart_send(serial_device, signature)

        # read results
        verification_results = serial_device.readline()
        eff_errors += verification_results[0]
        effprog_errors += verification_results[1]

        print(f'\rPROGRESS: {int((i+1) / SAMPLE_SIZE * 100)}%', end='')

    print('\r')

    print(f'Efficient verification error rate: {eff_errors / SAMPLE_SIZE * 100:.3f}%')
    print(f'Efficient+progressive verification error rate: {effprog_errors / SAMPLE_SIZE * 100:.3f}%', end = '\n\n')

    print(f'Average resulting vector time: {uart_readline(serial_device)} microseconds')
    print(f'Average efficient verification time: {uart_readline(serial_device)} microseconds')
    print(f'Average efficient+progressive verification time: {uart_readline(serial_device)} microseconds')
