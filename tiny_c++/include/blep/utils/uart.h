#ifndef UART_UTILS_H
#define UART_UTILS_H

#include <cstdint>

#include <zephyr/device.h>

#define SEGMENT_SIZE 32

extern unsigned char uart_rx_buffer[SEGMENT_SIZE];


unsigned char read_byte();
void read_n_bytes(unsigned char *buffer, unsigned int n, bool segmented=false);
uint32_t read_uint32();

void receive_n_bytes(unsigned int n);

void send_byte(unsigned char data);
void send_n_bytes(const unsigned char *data, unsigned int n);
void send_ack();
void send_ok();

#endif
