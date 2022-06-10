import serial
import subprocess
import secrets

BOARD = 'cc1352r1_launchxl'

print('Building project...')
build_process = subprocess.Popen(['west', 'build', '-p', '-b', BOARD], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
build_output = build_process.communicate()

if build_process.returncode != 0:
    print(build_output[0].decode('utf-8'))
    print(build_output[1].decode('utf-8'))
    exit(-1)

serial_device = serial.Serial('/dev/ttyACM0', 115200)
serial_device.write(secrets.token_bytes(4096*3))

application_process = subprocess.Popen(['west', 'flash'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)


print('Waiting for application...', end='\n\n')

application_process.communicate()

print(serial_device.readline())
print(serial_device.readline())

serial_device.close()