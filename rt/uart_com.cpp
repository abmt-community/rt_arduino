/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

#include "uart_com.h"
#include <avr/interrupt.h>
#include "rt/irq_helper.h"

#define SET_BIT(REG, BIT)  ((REG) |= (1 << BIT))
#define CLEAR_BIT(REG, BIT)  ((REG) &= (~(1 << BIT)))
#define GET_BIT(REG, BIT)  ((REG >> BIT) & 1)
#define UBRR(BAUD) (F_CPU/16/BAUD-1)

fifo<60> uart_in_buffer;
fifo<60> uart_out_buffer;
uint8_t out_bytes_to_pop = 0;

static bool sending = false;
static void uart_snd_next();


ISR(USART_TX_vect)
{
	if (uart_out_buffer.bytes_used > out_bytes_to_pop) {
		uart_snd_next();
	}else{
		sending = false;
	}
}

ISR(USART_RX_vect)
{
	if(uart_in_buffer.bytes_used < uart_in_buffer.size){
		uart_in_buffer.data[uart_in_buffer.bytes_used] = UDR0;
		uart_in_buffer.bytes_used++;
	}
}

void uart_init(uint32_t baudrate)
{
	// set speed
	UBRR0L = UBRR(baudrate) & 0xFF;
	UBRR0H = UBRR(baudrate) >> 8;

	// async
	CLEAR_BIT(UCSR0C, UMSEL00);
	CLEAR_BIT(UCSR0C, UMSEL01);
	
	// 8bit
	SET_BIT(UCSR0C, UCSZ00);
	SET_BIT(UCSR0C, UCSZ01);

	// no parrity
	CLEAR_BIT(UCSR0C, UPM00);
	CLEAR_BIT(UCSR0C, UPM01);

	// stop tbit
	CLEAR_BIT(UCSR0C, USBS0);
	
	// isr
	SET_BIT(UCSR0B, TXCIE0);
	SET_BIT(UCSR0B, RXCIE0);
	sei();

	// enable
	SET_BIT(UCSR0B, RXEN0);
	SET_BIT(UCSR0B, TXEN0);
}

void uart_write(const void *data, int size)
{
	if(size > uart_out_buffer.size){
		abmt::die("uart full");
	}
	do{ 
		auto lock = irq::scope_lock();
		uart_out_buffer.pop_front(out_bytes_to_pop);
		out_bytes_to_pop = 0;
	} while(uart_out_buffer.bytes_free() < size);

	uart_out_buffer.push_back(data, size);
	if (sending == false) {
		uart_snd_next();
	}
}

static void uart_snd_next()
{
	sending = true;
	UDR0 = uart_out_buffer.data[out_bytes_to_pop];
	out_bytes_to_pop++;
}