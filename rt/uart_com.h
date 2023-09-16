/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

#ifndef UART_COM_H_
#define UART_COM_H_


#include <string.h>
#include <cstdint>
#include <rt/fifo.h>

extern fifo<60> uart_in_buffer;
extern fifo<60> uart_out_buffer;

void uart_init(uint32_t);
void uart_write(const void *data, int size); /// may block until there is enough space in buffer

#endif /* UART_COM_H_ */
