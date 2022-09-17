#include <string.h>
#include <stdint.h>

#include <zephyr.h>
#include <device.h>
#include <drivers/uart.h>

#include "../include/uart_utils.h"

#define SEGMENT_SIZE 32
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
const unsigned char ACK[4] = {'A', 'C', 'K', '\n'};
const unsigned char OK[3] = {'O', 'K', '\n'};

/*
 * Read and return a single byte from UART.
 */
unsigned char read_byte(const struct device *dev) {

	unsigned char b;

	while (uart_poll_in(dev, &b) != 0) {}

	return b;
}

/*
 * Read n bytes from UART into the given buffer.
 */
void read_n_bytes(const struct device *dev, unsigned char *buffer, unsigned int n) {
	for (unsigned int i = 0; i < n; i++)
		buffer[i] = read_byte(dev);

	send_ack(dev);
}

void read_n_bytes_segmented(const struct device *dev, unsigned char *buffer, unsigned int n) {
	unsigned int n_chunks = (n >> 5) + 1, last_chunk_size = n & (SEGMENT_SIZE - 1);

	for (unsigned int i = 0; i < n_chunks - 1; i++) {
		for (unsigned int j = 0; j < SEGMENT_SIZE; j++)
			buffer[i*SEGMENT_SIZE + j] = read_byte(dev);
		
		// send ACK for the given chunk
		send_ack(dev);
	}

	if (last_chunk_size != 0) {
		for (unsigned int j = 0; j < last_chunk_size; j++)
			buffer[(n_chunks - 1) * SEGMENT_SIZE + j] = read_byte(dev);
			
		send_ack(dev);
	}
}

uint32_t read_uint32(const struct device *dev) {
    uint32_t value = 0;
	send_ok(dev);

    for (unsigned short i = 0; i < 12; i+=3)
		value |= read_byte(dev) << i;
	
	send_ack(dev);
    return value;
}

/*
 * Send n bytes to the UART interface
 */
void send_n_bytes(const struct device *dev, const unsigned char *message, unsigned int n) {
	for (unsigned int i = 0; i < n; i++)
		uart_poll_out(dev, message[i]);
}

void send_ack(const struct device *dev) {
	send_n_bytes(dev, ACK, 4);
}

void send_ok(const struct device *dev) {
	send_n_bytes(dev, OK, 3);
}
