#ifndef __OPENMV_H
#define __OPENMV_H

#include "./SYSTEM/sys/sys.h"       

#define OPENMV_REC_LEN      32
#define OPENMV_RX_BUF_SIZE  1

extern UART_HandleTypeDef g_uart2_openmv_handle;

extern uint8_t openmv_rx_buf[OPENMV_REC_LEN];
extern uint8_t openmv_rx_buffer[OPENMV_RX_BUF_SIZE];
extern volatile uint16_t openmv_rx_sta;

extern volatile uint8_t openmv_state;
extern volatile int16_t openmv_offset;
extern volatile uint8_t openmv_data_ready;

void openmv_uart2_init(uint32_t baudrate);
void openmv_parse_data(void);

#endif

/*
openmv传回的偏移量通过改变target,target通过定速pid控制小车实现转速改变
openmv传回的偏移量 > 0:线在openmv右侧,需使左轮加速,右轮减速
openmv传回的偏移量 < 0:线在openmv左侧,需使右轮加速,左轮减速
*/
