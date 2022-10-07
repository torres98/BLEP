from time import sleep
import serial
import subprocess

from effprog.ver.effver import generate_random_linear_transformation
from effprog.uart_utils import *
from effprog.parsing_utils import parse_args

SHA_CHUNK_SIZE = 32678
TTY_PORT = 115200

shell_args = parse_args()

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

PK_PATH = f'/home/torres/Desktop/Thesis/verification_implementation/tmp/pk{shell_args["RAINBOW_VERSION"]}.txt'
SIG_PATH = f'/home/torres/Desktop/Thesis/verification_implementation/tmp/signature{shell_args["RAINBOW_VERSION"]}.txt'
MSG_PATH = '/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb'


#Generate random transformation and corresponding svk
print('Creating SVK...', end=' ', flush=True)

PK = Rainbow.parse_public_key(PK_PATH)
C = generate_random_linear_transformation(shell_args['SVK_NROWS'], Rainbow.n_polynomials, 0, q-1, gf)
SVK = C.dot(PK)

f = open('src/svk.h', 'w')

if shell_args['RAINBOW_VERSION'] == 1:
    f.write(f'#include <stdint.h>\n\n#define SVK_NROWS {shell_args["SVK_NROWS"]}\n\nconst uint8_t short_private_key[{SVK.size // 2}] = {{')

    for i in range(SVK.shape[0]):
        for j in range(0, SVK.shape[1], 2):
            f.write(f'{(int(SVK[i, j+1]) << 4) + int(SVK[i, j])}, ')
            
    f.write(f'}};\n\nconst uint8_t private_transformation[{C.size // 2}] = {{')

    for i in range(C.shape[0]):
        for j in range(0, C.shape[1], 2):
            f.write(f'{(int(C[i, j+1]) << 4) + int(C[i, j])}, ')

else:
    f.write(f'#include <stdint.h>\n\n#define SVK_NROWS {shell_args["SVK_NROWS"]}\n\nconst uint8_t short_private_key[{SVK.size}] = {{')

    for elem in SVK.flat:
        f.write(f'{elem}, ')
    
    f.write(f'}};\n\nconst uint8_t private_transformation[{C.size}] = {{')

    for elem in C.flat:
        f.write(f'{elem}, ')

f.write('};\n')
f.close()

"""build_process = subprocess.Popen([f'./generate_rainbow_svk_v{VERSION}.o', K, '>' , 'src/svk.h'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
build_output = build_process.communicate()

if build_process.returncode != 0:
    print(build_output[0].decode('utf-8'))
    print(build_output[1].decode('utf-8'))
    exit(-1)
"""
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
        f'-DGF{q}_LOOKUP={shell_args["LOOKUP_LEVEL"]}',
        '-DELEM_BIT_SIZE=4' if shell_args["RAINBOW_VERSION"] == 1 else ''
    ]

    build_process = subprocess.Popen(build_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    build_output = build_process.communicate()

    if build_process.returncode != 0:
        print(build_output[0].decode('utf-8'))
        print(build_output[1].decode('utf-8'))
        exit(-1)

    print('DONE')
    print(f'Memory region{build_output[0].decode("utf-8").split("Memory region")[1]}')

subprocess.check_call(shell_args['FLASH_CMD'])

sleep(0.1)

#LAUNCH APPLICATION
with serial.Serial(shell_args['TTY_DEVICE'], TTY_PORT) as serial_device:
    
    print()
    print(uart_readline(serial_device))

    uart_send_int(serial_device, shell_args['PROGRESSIVE_STEPS'])

    #SIGNATURE
    with open(SIG_PATH, 'rb') as signature_file:
        #consume header
        while(signature_file.read(1) != b'='): pass
        signature_file.read(1)

        signature_raw_str = bytes.fromhex(signature_file.read()[:-1].decode('utf-8'))

    uart_wait(serial_device)
    uart_send(serial_device, signature_raw_str)
    print('Signature correctly sent.')

    #MESSAGE
    with open(MSG_PATH, 'rb') as message_file:
        message = message_file.read()

    #send message length (encoded in little endian)
    mlen = len(message)
    uart_send_int(serial_device, mlen)
    print('Message length correctly sent.')

    #receive gothrough
    uart_wait(serial_device)

    #send message
    for i in range(0, mlen, SHA_CHUNK_SIZE):
        uart_send_segmented(serial_device, message[i: i + SHA_CHUNK_SIZE])

    print('Message correctly sent.')

    salt_raw_str = signature_raw_str[-16:]
    message_digest = Rainbow.message_digest(MSG_PATH, salt_raw_str) #magari usa direttamente la stringa visto che ce l'hai

    assert(message_digest == uart_readline(serial_device))
    print('Digest correctly computed\n')

    print(f'Efficient Verification: {serial_device.readline()[:-3].decode("utf-8")}')
    print(f'Efficient+Progressive Verification: {serial_device.readline()[:-3].decode("utf-8")}')
