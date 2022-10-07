#include <cstdint>

#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>

#include "../include/uart_utils.h"

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

unsigned char uart_rx_buffer[SEGMENT_SIZE];
const unsigned char ACK[4] = {'A', 'C', 'K', '\n'};
const unsigned char OK[3] = {'O', 'K', '\n'};


/*
 * Read and return a single byte from UART.
 */
unsigned char read_byte() {
	unsigned char b;

	while (uart_poll_in(uart_dev, &b) != 0) {}

	return b;
}

/*
 * Read n bytes from UART into the given buffer.
 */
void read_n_bytes(unsigned char *buffer, unsigned int n) {
	for (unsigned int i = 0; i < n; i++)
		buffer[i] = read_byte();

	send_ack();
}

void read_n_bytes_segmented(unsigned char *buffer, unsigned int n) {
	unsigned int n_chunks = (n >> 5) + 1, last_chunk_size = n & (SEGMENT_SIZE - 1);

	for (unsigned int i = 0; i < n_chunks - 1; i++) {
		for (unsigned int j = 0; j < SEGMENT_SIZE; j++)
			buffer[i*SEGMENT_SIZE + j] = read_byte();
		
		// send ACK for the given chunk
		send_ack();
	}

	if (last_chunk_size != 0) {
		for (unsigned int j = 0; j < last_chunk_size; j++)
			buffer[(n_chunks - 1) * SEGMENT_SIZE + j] = read_byte();
			
		send_ack();
	}
}


void receive_n_bytes(unsigned short n) {
	for (uint16_t i = 0; i < n; i++)
		uart_rx_buffer[i] = read_byte();

	send_ack();
}

void receive_segment() {
	receive_n_bytes(SEGMENT_SIZE);
}

uint32_t read_uint32() {
    uint32_t value = 0;
	send_ok();

    for (unsigned short i = 0; i < 32; i+=8)
		value |= read_byte() << i;
	
	send_ack();

    return value;
}

/*
 * Send n bytes to the UART interface
 */
void send_n_bytes(const unsigned char *message, unsigned int n) {
	for (unsigned int i = 0; i < n; i++)
		uart_poll_out(uart_dev, message[i]);
}

void send_ack() {
	send_n_bytes(ACK, 4);
}

void send_ok() {
	send_n_bytes(OK, 3);
}
