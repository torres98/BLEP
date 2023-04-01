import os
import secrets
import serial
import signal
import subprocess
import threading

from effprog.ver.effver import generate_random_linear_transformation
from effprog.uart_utils import *
from effprog.parsing_utils import parse_args

SHA_CHUNK_SIZE = 32678
TTY_PORT = 115200

GF_LOOKUP_LEVELS_CONVERSION_TABLE = {
    '0': 0,
    '1': 1,
    '1r': 2,
    '2': 3,
    '2r': 4
}

def read_zephyr_output(proc_stdout):
    output_line = proc_stdout.readline()

    while(not output_line.startswith(b'*** Booting')):
        output_line = proc_stdout.readline()

    for line in iter(proc_stdout.readline, b''):
        print(line.decode('utf-8'), end='')

# <sig> and <frame> args are not needed
def interrupt_signal_handler(*_):
    global zephyr_process
    os.killpg(os.getpgid(zephyr_process.pid), signal.SIGTERM)
    
    global read_output_thread
    read_output_thread.join()
    print('\nThank you for using BLEP, bye!')

def find_tty_device():
    serial_device = None

    print(os.listdir('/dev/'))

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

################################
# launch.py script starts here #
################################

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
MSG_PATH = '/home/torres/Desktop/Thesis/verification_implementation/tmp/debug.gdb'

# BUILD APPLICATION
# if rebuild has been requested or there is no build directory...
if not shell_args['SKIP_BUILD']:
    # Generate random transformation and corresponding svk
    print('Creating SVK...', end=' ', flush=True)

    PK = Rainbow.parse_public_key(PK_PATH)
    C = generate_random_linear_transformation(shell_args['SVK_NROWS'], Rainbow.n_polynomials, 0, q-1, gf)
    SVK = C.dot(PK)

    # write svk to header file
    with open('src/svk.h', 'w') as svk_file:
        svk_file.write(f'#include <stdint.h>\n\n#define SVK_NROWS {shell_args["SVK_NROWS"]}\n\nconst uint8_t short_private_key[{SVK.size}] = {{')

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

    build_cmd = [
        'west',
        'build',
        '-p=auto',
        f'-b={shell_args["BOARD"]}',
        '--',
        f'-DRAINBOW_VERSION={RAINBOW_VERSION}',
        f'-DGF{q}_LOOKUP={GF_LOOKUP_LEVELS_CONVERSION_TABLE[shell_args["LOOKUP_LEVEL"]]}',
        #used only by qemu
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

signal.signal(signal.SIGINT, interrupt_signal_handler)

print("""
*************************************************************
****To interrupt the application press CTRL+C at any time****
*************************************************************
""")
      
# LAUNCH APPLICATION
if shell_args['BOARD'].startswith('qemu'):
    zephyr_process = subprocess.Popen(shell_args['FLASH_CMD'], stdout=subprocess.PIPE, preexec_fn=os.setsid)

    read_output_thread = threading.Thread(target=read_zephyr_output, args=(zephyr_process.stdout,))
    read_output_thread.start()
else:
    try:
        subprocess.check_call(shell_args['FLASH_CMD'])
    except subprocess.CalledProcessError:
        print('Flash command failed. No tty device found!')
        exit(-1)
    
    with find_tty_device() as serial_device:
        print()

        # discard preamble lines
        line_discarded = serial_device.readline()

        while(not line_discarded.startswith(b'*** Booting Zephyr OS')):
            line_discarded = serial_device.readline()
    
        uart_send_int(serial_device, shell_args['PROGRESSIVE_STEPS'])
        uart_send_int(serial_device, SAMPLE_SIZE)

        # MESSAGE
        with open(MSG_PATH, 'rb') as message_file:
            message = message_file.read()

        mlen = len(message)
        uart_send_int(serial_device, mlen)
        print('Message length correctly sent.')

        # receive gothrough
        uart_wait(serial_device)

        for i in range(0, mlen, SHA_CHUNK_SIZE):
            uart_send_segmented(serial_device, message[i: i + SHA_CHUNK_SIZE])

        print('Message correctly sent.')

        eff_errors = 0
        effprog_errors = 0

        for i in range(SAMPLE_SIZE):
            # generate random signature
            signature = secrets.token_bytes(Rainbow.n_variables // (2 // Rainbow.element_hex_size))

            # receive gothrough
            uart_wait(serial_device)
            uart_send_segmented(serial_device, signature)

            # generate random salt 
            salt = secrets.token_bytes(16)
            uart_send_segmented(serial_device, salt)
            
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
