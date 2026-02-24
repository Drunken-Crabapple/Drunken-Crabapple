#ifndef _USART_H
#define _USART_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"

extern UART_HandleTypeDef g_usart1_handle;
extern uint8_t g_usart1_rx_buffer[1];
extern uint16_t g_usart1_rx_sta;
extern uint8_t g_usart1_rx_buf[200];

void usart1_init(uint32_t baudrate);


#endif
