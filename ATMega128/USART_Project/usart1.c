#include <avr/io.h>
#include "usart1.h"

void Init_USART1(void)
{
	DDRE |= (1 << PE1);    // TX1
	DDRE &= ~(1 << PE0);   // RX1

	UCSR1A = 0x00;
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);  // 8bit

	UBRR1H = 0x00;
	UBRR1L = 0x07;   // 115200bps @ 14.7456MHz
}

void USART1_tx(unsigned char data)
{
	while (!(UCSR1A & (1 << UDRE1)));
	UDR1 = data;
}

unsigned char USART1_rx(void)
{
	while (!(UCSR1A & (1 << RXC1)));
	return UDR1;
}

void USART1_str(const char* str)
{
	while (*str)
	{
		USART1_tx(*str++);
	}
}
