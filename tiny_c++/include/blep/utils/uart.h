#ifndef UART_UTILS_H
#define UART_UTILS_H

#include <cstdint>

#include <zephyr/device.h>

#define SEGMENT_SIZE 32

extern unsigned char uart_rx_buffer[SEGMENT_SIZE];


unsigned char read_byte();
void read_n_bytes(unsigned char *buffer, unsigned int n);
void read_n_bytes_segmented(unsigned char *buffer, unsigned int n);

uint32_t read_uint32();

void send_n_bytes(const unsigned char *message, unsigned int n);
void send_ack();
void send_ok();


void receive_segment();

void receive_n_bytes(unsigned short n);

#endif
