#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/WDG/wdg.h"



int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    usart1_init(115200);    
    
    char ch[] = "您还没有喂狗，请及时喂狗！！！\r\n";
    char wh[] = "您喂过狗了\r\n";
    HAL_UART_Transmit(&g_usart1_handle,(uint8_t *)ch,strlen(ch),100);
    
    iwdg_init(IWDG_PRESCALER_16,2000);  /* 预分频系数为16，重装载值为2000，溢出时间约为1s */

    
    while(1)
    {
        delay_ms(940);
        iwdg_feed();
        HAL_UART_Transmit(&g_usart1_handle,(uint8_t *)wh,strlen(wh),100);
        
    }
}

