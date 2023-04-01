import os
import serial
import subprocess

from effprog.ver.effver import generate_random_linear_transformation
from effprog.uart_utils import *
from effprog.parsing_utils import parse_args

SHA_CHUNK_SIZE = 32678
TTY_PORT = 115200

def find_tty_device():
    serial_device = None

    for filename in os.listdir('/dev/'):
        if filename.startswith('tty'):
            try:
                serial_device = serial.Serial(f'/dev/{filename}', TTY_PORT)
            except serial.SerialException:
                pass

            break

    if serial_device is None:
        raise serial.SerialException("No tty device found.")

    return serial_device


shell_args = parse_args()

RAINBOW_VERSION = shell_args['RAINBOW_VERSION']

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

PK_PATH = f'/home/torres/Desktop/Thesis/verification_implementation/tmp/pk{RAINBOW_VERSION}.txt'
SIG_PATH = f'/home/torres/Desktop/Thesis/verification_implementation/tmp/signature{RAINBOW_VERSION}.txt'
MSG_PATH = '/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb'


#BUILD APPLICATION
#if rebuild has been requested or there is no build directory...
if not shell_args['SKIP_BUILD']:
    #Generate random transformation and corresponding svk
    print('Creating SVK...', end=' ', flush=True)

    PK = Rainbow.parse_public_key(PK_PATH)
    C = generate_random_linear_transformation(shell_args['SVK_NROWS'], Rainbow.n_polynomials, 0, q-1, gf)
    SVK = C.dot(PK)

    with open('src/svk.h', 'w') as svk_file:
        svk_file.write(f'#include <cstdint>\n\n#define SVK_NROWS {shell_args["SVK_NROWS"]}\n\nconst uint8_t short_private_key[{SVK.size}] = {{')

        for i in range(SVK.shape[0]):
            for j in range(SVK.shape[1]):
                svk_file.write(f'{SVK[i, j]}, ')

        svk_file.write(f'}};\n\nconst uint8_t private_transformation[{C.size}] = {{')

        for i in range(C.shape[0]):
            for j in range(C.shape[1]):
                svk_file.write(f'{C[i, j]}, ')

        svk_file.write('};\n')

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
        '-p=auto',
        f'-b={shell_args["BOARD"]}',
        '--',
        f'-DRAINBOW_VERSION={RAINBOW_VERSION}',
        f'-DGF{q}_LOOKUP={gf_lookup_level[shell_args["LOOKUP_LEVEL"]]};'
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
subprocess.check_call(shell_args['FLASH_CMD'])

with find_tty_device() as serial_device:
    print()

    # discard preamble lines
    line_discarded = serial_device.readline()

    while(not line_discarded.endswith(f'*** Booting Zephyr OS build {"v3.2.0-rc2"}  ***\r\n'.encode())):
        line_discarded = serial_device.readline()

    uart_send_int(serial_device, shell_args['PROGRESSIVE_STEPS'])

    #SIGNATURE
    with open(SIG_PATH, 'rb') as signature_file:
        #consume header
        while(signature_file.read(1) != b'='): pass
        signature_file.read(1)

        signature_raw_str = bytes.fromhex(signature_file.read()[:-1].decode('utf-8'))

    #receive gothrough
    uart_wait(serial_device)
    uart_send_segmented(serial_device, signature_raw_str[:-16])

    # send salt 
    uart_send_segmented(serial_device, signature_raw_str[-16:])

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
