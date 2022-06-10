from time import sleep

import argparse
import serial
import subprocess

from effprog.ver.effver import generate_random_linear_transformation
from effprog.uart_utils import uart_send, uart_send_segmented
from effprog.parsing_utils import parse_args

SHA_CHUNK_SIZE = 32678

PK_PATH = '/home/torres/Desktop/Thesis/verification_implementation/tmp/pk.txt'
MSG_PATH = '/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb'
SIG_PATH = '/home/torres/Desktop/Thesis/verification_implementation/tmp/signature.txt'

VERSION, BOARD, TTY_DEVICE, FLASH_CMD, K, T = parse_args()

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


#Generate random transformation and corresponding svk
print('Creating SVK...', end=' ', flush=True)

PK = Rainbow.parse_public_key(PK_PATH)
C = generate_random_linear_transformation(K, Rainbow.n_polynomials, 0, q-1, gf)
SVK = C.dot(PK)

f = open('src/svk.h', 'w')
f.write(f'#include "tiny/include/gf{q}.h"\n\n#define k {K}u\n\nconst uint8_t short_private_key[{SVK.size}] = {{')

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

build_process = subprocess.Popen(['west', 'build', '-p', '-b', BOARD, '--', f'-DVERSION={VERSION}'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
build_output = build_process.communicate()

if build_process.returncode != 0:
    print(build_output[0].decode('utf-8'))
    print(build_output[1].decode('utf-8'))
    exit(-1)

print('DONE')

#LAUNCH APPLICATION
print('Waiting for application...', end=' ', flush=True)
subprocess.check_call(FLASH_CMD, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
print('DONE', end = '\n\n')

serial_device = serial.Serial(TTY_DEVICE, 115200)

serial_device.readline()
sleep(0.1)

#SIGNATURE
with open(SIG_PATH, 'rb') as signature_file:
    #consume header
    while(signature_file.read(1) != b'='): pass
    signature_file.read(1)

    signature_raw_str = bytes.fromhex(signature_file.read()[:-1].decode('utf-8'))

uart_send(serial_device, signature_raw_str)
print('Signature correctly sent.')


#MESSAGE
with open(MSG_PATH, 'rb') as message_file:
    message = message_file.read()

#send message length (encoded in little endian)
mlen = len(message)
uart_send(serial_device, mlen.to_bytes(4, 'little'))
print('Message length correctly sent.')

#send message
for i in range(0, mlen, SHA_CHUNK_SIZE):
    uart_send_segmented(serial_device, message[i: i + SHA_CHUNK_SIZE])

print('Message correctly sent.')


salt_raw_str = signature_raw_str[-16:]
message_digest = Rainbow.message_digest(MSG_PATH, salt_raw_str) #magari usa direttamente la stringa visto che ce l'hai

print('Final message digest:')
print(message_digest)
print(serial_device.readline()[:-3].decode("utf-8"))

print(f'Efficient Verification: {serial_device.readline()[:-3].decode("utf-8")}')
print(f'Efficient+Progressive Verification: {serial_device.readline()[:-3].decode("utf-8")}')

serial_device.close()
