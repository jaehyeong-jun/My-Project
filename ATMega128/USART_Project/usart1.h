#ifndef USART1_H_
#define USART1_H_

void Init_USART1(void);
void USART1_tx(unsigned char data);
unsigned char USART1_rx(void);
void USART1_str(const char* str);

#endif
