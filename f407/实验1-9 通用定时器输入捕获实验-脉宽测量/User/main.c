#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/TIMER/gtim.h"



int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    gtim_tim5_icap_ch1_init(84 - 1,0xFFFF);
    usart_init(115200);
    
    extern uint8_t g_timxchy_cap_sta;
    extern uint16_t g_timxchy_cap_val;
    int32_t temp = 0;
    int16_t t = 0;
    
    while(1)
    {
        if(g_timxchy_cap_sta & 0x80)        //两种情况，一是正常捕获高电平脉冲，二是持续时间过长
        {
            if(g_timxchy_cap_val == 0xFFFF)
            {
                printf("高电平持续时间过长 \r\n");
            }
            else
            {
                t = g_timxchy_cap_sta & 0x3F;
                temp = t * 65536 + g_timxchy_cap_val;
                printf("高电平持续时间为：%d \r\n",temp);
            }
            g_timxchy_cap_sta = 0;
            g_timxchy_cap_val = 0;
        }
    }
}

