SEGMENT_SIZE = 32

def uart_send_segmented(serial_device, msg):
    mlen = len(msg)

    for i in range(0, mlen, SEGMENT_SIZE):
        serial_device.write(msg[i: i + SEGMENT_SIZE])
        reply = serial_device.readline()
        assert reply == b'OK\n', f'Received unexpected reply ({reply})' #wait for confermation

def uart_send(serial_device, msg):
    serial_device.write(msg)
    reply = serial_device.readline()
    assert reply == b'OK\n', f'Received unexpected reply ({reply})' #wait for confermation
