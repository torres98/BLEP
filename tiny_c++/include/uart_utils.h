#ifndef UART_UTILS_H
#define UART_UTILS_H

#include <cstdint>

#include <device.h>

extern const struct device *uart_dev;

unsigned char read_byte(const struct device *dev);
void read_n_bytes(const struct device *dev, unsigned char *buffer, unsigned int n);
void read_n_bytes_segmented(const struct device *dev, unsigned char *buffer, unsigned int n);

uint32_t read_uint32(const struct device *dev);

void send_n_bytes(const struct device *dev, const unsigned char *message, unsigned int n);
void send_ack(const struct device *dev);

#endif
