/**
 ****************************************************************************************************
 * @file        usart.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2023-06-05
 * @brief       串口初始化代码(一般是串口1)，支持printf
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211014
 * 第一次发布
 * V1.1 20230605
 * 删除USART_UX_IRQHandler()函数的超时处理和修改HAL_UART_RxCpltCallback()
 ****************************************************************************************************
 */

#ifndef _USART_H
#define _USART_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"


extern uint8_t g_usart1_rx_flag ; //声明是不能赋值的
extern uint8_t g_rx_buffer[1];
extern UART_HandleTypeDef g_uart1_handle;

void usart_init(uint32_t baudrate);

#endif







