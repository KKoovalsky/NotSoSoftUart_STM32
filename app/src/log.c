#include "log.h"
#include "usart.h"

#define USART_INST USART1

void uart_send_byte(uint8_t byte)
{
	while(LL_USART_IsActiveFlag_TXE(USART_INST) == RESET);
	LL_USART_TransmitData8(USART_INST, byte);
}

void log_byte(uint8_t byte)
{
	uart_send_byte(byte);
}
