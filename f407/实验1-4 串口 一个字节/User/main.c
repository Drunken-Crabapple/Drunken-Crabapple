/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-14
 * @brief       template 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台：正点原子 探索者 F407开发板
 * 在线视频：www.yuanzige.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"




int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336,8,2,7);
    delay_init(168);
    
    usart_init(115200);
    
    while(1)
    {
        if(g_usart1_rx_flag == 1)  //把接收到的数据发送出去
        {
            HAL_UART_Transmit(&g_uart1_handle,(uint8_t*)g_rx_buffer,1,100);
            while(__HAL_UART_GET_FLAG(&g_uart1_handle,UART_FLAG_TC) != 1);  //为一即发送完毕
            printf("Z\r\n");
            g_usart1_rx_flag = 0;
        }
        else
        {
            delay_ms(10);
            
        }
        
    }
}

