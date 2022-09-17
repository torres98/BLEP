def uart_wait(serial_device):
    msg = serial_device.readline()
    assert msg == b'OK\n', f'Received unexpected message ({msg})' #wait for confermation

def uart_readline(serial_device):
    return serial_device.readline()[:-3].decode('utf-8')

def uart_send_segmented(serial_device, msg):
    SEGMENT_SIZE = 32
    mlen = len(msg)

    for i in range(0, mlen, SEGMENT_SIZE):
        serial_device.write(msg[i: i + SEGMENT_SIZE])
        reply = serial_device.readline()
        assert reply == b'ACK\n', f'Received unexpected reply ({reply})' #wait for confermation

def uart_send(serial_device, msg):
    serial_device.write(msg)
    reply = serial_device.readline()
    assert reply == b'ACK\n', f'Received unexpected reply ({reply})' #wait for confermation

def uart_send_int(serial_device, value, size = 4, endianess = 'little'):
    uart_wait(serial_device)

    serial_device.write(value.to_bytes(size, endianess))
    reply = serial_device.readline()
    assert reply == b'ACK\n', f'Received unexpected reply ({reply})' #wait for confermation
