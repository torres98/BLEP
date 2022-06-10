from time import sleep

import serial
import subprocess
import secrets

from effprog.ver.effver import generate_random_linear_transformation
from effprog.uart_utils import uart_send, uart_send_segmented
from effprog.parsing_utils import parse_args

SHA_CHUNK_SIZE = 32678

PK_PATH = '/home/torres/Desktop/Thesis/verification_implementation/tmp/pk.txt'
MSG_PATH = '/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb'

VERSION, BOARD, TTY_DEVICE, FLASH_CMD, K, T = parse_args()
TTY_PORT = 115200

SAMPLE_SIZE = 10000

if VERSION == 1:
    from effprog.schemes.rainbow import RainbowI as Rainbow
    from effprog.schemes.gf.gf16 import gf16 as gf

    q = 16

elif VERSION == 2:
    from effprog.schemes.rainbow import RainbowIII as Rainbow
    from effprog.schemes.gf.gf256 import gf256 as gf

    q = 256

else:
    from effprog.schemes.rainbow import RainbowV as Rainbow
    from effprog.schemes.gf.gf256 import gf256 as gf

    q = 256


#generate random transformation and corresponding svk
print('Creating SVK...', end=' ', flush=True)

PK = Rainbow.parse_public_key(PK_PATH)
C = generate_random_linear_transformation(K, Rainbow.n_polynomials, 0, q-1, gf)
SVK = C.dot(PK)

f = open('src/svk.h', 'w')
f.write(f'const uint8_t short_private_key[{SVK.size}] = {{')

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
print('Building project...', end=' ', flush=True)

build_process = subprocess.Popen(
    ['west', 'build', '-p', '-b', BOARD, '--', f'-DVERSION={VERSION}', f'-DSAMPLE_SIZE={SAMPLE_SIZE}', f'-DSVK_NROWS={K}', f'-DPROGRESSIVE_STEPS={T}'],
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE
)
build_output = build_process.communicate()

if build_process.returncode != 0:
    print(build_output[0].decode('utf-8'))
    print(build_output[1].decode('utf-8'))
    exit(-1)

print('DONE')

#LAUNCH APPLICATION
serial_device = serial.Serial(TTY_DEVICE, TTY_PORT)

print('Waiting for application...', end=' ', flush=True)
subprocess.check_call(FLASH_CMD, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
print('DONE', end = '\n\n')

serial_device.readline()
sleep(0.05) # now, i know that sounds bad...

#MESSAGE
with open(MSG_PATH, 'rb') as message_file:
    message = message_file.read()

mlen = len(message)
uart_send(serial_device, mlen.to_bytes(4, 'little'))
print('Message length correctly sent.')

for i in range(0, mlen, SHA_CHUNK_SIZE):
    uart_send_segmented(serial_device, message[i: i + SHA_CHUNK_SIZE])

print('Message correctly sent.')
sleep(0.05)

eff_errors = 0
effprog_errors = 0

for i in range(SAMPLE_SIZE):
    # generate random signature
    signature = secrets.token_bytes(Rainbow.n_variables // (2 // Rainbow.element_hex_size) + 16)
    uart_send(serial_device, signature)

    # read results
    results = serial_device.readline()
    eff_errors += results[0]
    effprog_errors += results[1]

    print(f'\rPROGRESS: {int((i+1) / SAMPLE_SIZE * 100)}%', end='')
    sleep(0.005)

print('\r')

print(f'Efficient verification error rate: {eff_errors / SAMPLE_SIZE * 100}%')
print(f'Efficient+progressive verification error rate: {effprog_errors / SAMPLE_SIZE * 100}%')

print(serial_device.readline()[:-3].decode("utf-8"))
print(serial_device.readline()[:-3].decode("utf-8"))

serial_device.close()
