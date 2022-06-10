import serial
import subprocess

process = subprocess.Popen(['west', 'flash'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=None)

print('Waiting for application...', end='\n\n')

process_output = process.communicate()

if process.returncode != 0:
    print(process_output[0].decode('utf-8'))
    print(process_output[1].decode('utf-8'))
    exit(-1)

print(ser.readline().decode('utf-8'), end='')
print(ser.readline().decode('utf-8'), end='')

user_input = ''

while user_input != 'q!':
    print(ser.readline().decode('utf-8'), end='')
    user_input = input()
    ser.write(f'{user_input}\n'.encode('utf-8'))

ser.close()