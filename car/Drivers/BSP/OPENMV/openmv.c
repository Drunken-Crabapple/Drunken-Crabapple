#include "./BSP/OPENMV/openmv.h"
#include <string.h>
#include <stdlib.h>

UART_HandleTypeDef g_uart2_openmv_handle;

uint8_t openmv_rx_buf[OPENMV_REC_LEN];
uint8_t openmv_rx_buffer[OPENMV_RX_BUF_SIZE];
volatile uint16_t openmv_rx_sta = 0;

volatile uint8_t openmv_state = 0;
volatile int16_t openmv_offset = 0;
volatile uint8_t openmv_data_ready = 0;

void openmv_uart2_init(uint32_t baudrate)
{
    g_uart2_openmv_handle.Instance = USART2;
    g_uart2_openmv_handle.Init.BaudRate = baudrate;
    g_uart2_openmv_handle.Init.WordLength = UART_WORDLENGTH_8B;
    g_uart2_openmv_handle.Init.StopBits = UART_STOPBITS_1;
    g_uart2_openmv_handle.Init.Parity = UART_PARITY_NONE;
    g_uart2_openmv_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_uart2_openmv_handle.Init.Mode = UART_MODE_TX_RX;

    HAL_UART_Init(&g_uart2_openmv_handle);
    HAL_UART_Receive_IT(&g_uart2_openmv_handle,(uint8_t *)openmv_rx_buffer,OPENMV_RX_BUF_SIZE);
}

/*
    数据解析
*/
void openmv_parse_data(void)
{
    char *comma;

    if((openmv_rx_sta & 0x8000) == 0)               //还未完成数据接收，不进行后续处理
    {
        return;
    }

    openmv_rx_buf[openmv_rx_sta & 0x3fff] = '\0';   //化为合法字符串

    comma = strchr((char *)openmv_rx_buf,',');      //找','的位置,指针指向','
    if(comma != NULL)                               //找到了
    {
        *comma = '\0';                              //把指针指向的','用'\0'代替
        openmv_state = (uint8_t)atoi((char *)openmv_rx_buf);    //因为把','换为'\0'即字符串里面的合法结束标志,所以这里实际只有前面第一个字节
        openmv_offset = (int16_t)atoi(comma + 1);
        openmv_data_ready = 1;                      //此位置一 说明已经接收到openmv传回的数据并已经完成数据解析 可以进行下一步处理
    }

    openmv_rx_sta = 0;
}
