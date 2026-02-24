#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"




/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1
#if (__ARMCC_VERSION >= 6010050)                    /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");          /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");            /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

/* FILE 在 stdio.h里面定义. */
FILE __stdout;

/* 重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);               /* 等待上一个字符发送完成 */

    USART1->DR = (uint8_t)ch;                       /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}
#endif
/***********************************************END*******************************************/
    


UART_HandleTypeDef g_usart1_handle;          /* 句柄 */
uint8_t g_usart1_rx_buffer[1];
uint16_t g_usart1_rx_sta = 0;
uint8_t g_usart1_rx_buf[200];

void usart1_init(uint32_t baudrate)
{
    g_usart1_handle.Instance = USART1;
    g_usart1_handle.Init.BaudRate = baudrate;
    g_usart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_usart1_handle.Init.Mode = UART_MODE_TX_RX;
    g_usart1_handle.Init.Parity = UART_PARITY_NONE;
    g_usart1_handle.Init.StopBits = UART_STOPBITS_1;
    g_usart1_handle.Init.WordLength = UART_WORDLENGTH_8B;
    HAL_UART_Init(&g_usart1_handle);
    
    HAL_UART_Receive_IT(&g_usart1_handle,(uint8_t *)g_usart1_rx_buffer,1);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;
    if(huart ->Instance == USART1)
    {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        gpio_init_struct.Pin = GPIO_PIN_9;      /* USART1_TX */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA,&gpio_init_struct);
        
        gpio_init_struct.Pin = GPIO_PIN_10;     /* USART1_RX */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA,&gpio_init_struct);
        
        HAL_NVIC_SetPriority(USART1_IRQn,2,0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&g_usart1_handle);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        if((g_usart1_rx_sta & 0x8000) == 0)          /* 第十五位还未置一，代表还未接收到换行\n 0x0a 数据还在传输*/
        {
            if((g_usart1_rx_sta & 0x4000))      /* 第十四位已经置一，说明已经接收到回车\r 0x0d 下一位一定是换行，否则错误 */
            {
                if(g_usart1_rx_buffer[0] != 0x0a)          /* 当前数据缓冲区中的数据不是换行 */
                {
                    g_usart1_rx_sta = 0;             /* 清除状态，重新开始接收 */
                }
                else                                /* 是换行，接收完毕 */
                {
                    g_usart1_rx_sta |= 0x8000;
                }
            }
            else                                    /* 第十四位还未置一，数据还在传输 */
            {
                if(g_usart1_rx_buffer[0] == 0x0d)          /*下一个数据就是回车 */
                {
                    g_usart1_rx_sta |= 0x4000;
                }
                else                                /* 下一个数据任然不是回车 */
                {
                    g_usart1_rx_buf[g_usart1_rx_sta & 0x3fff] = g_usart1_rx_buffer[0];
                    g_usart1_rx_sta++;
                    if(g_usart1_rx_sta > (200 - 1))
                    {
                        g_usart1_rx_sta = 0;
                    }
                }
            }
        }
        
        HAL_UART_Receive_IT(&g_usart1_handle,(uint8_t *)g_usart1_rx_buffer,1);
    }
}    


 

 




