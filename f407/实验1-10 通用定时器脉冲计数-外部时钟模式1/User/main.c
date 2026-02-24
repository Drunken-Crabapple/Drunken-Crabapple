#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/TIMER/gtim.h"


extern TIM_HandleTypeDef g_timx_cnt_chy_handle;

int main(void)
{
    uint16_t curcnt = 0;
    uint16_t oldcnt = 0;
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    gtim_tim2_ch1_init(0);
    usart_init(115200);
    
    while(1)
    {
        curcnt = __HAL_TIM_GET_COUNTER(&g_timx_cnt_chy_handle);
        if(oldcnt != curcnt)            //只有更新计数值才打印，避免重复打印
        {
            oldcnt = curcnt;
            printf("脉冲数为：%d \r\n",oldcnt);
            delay_ms(10);
            delay_ms(20);
        }
    }
}

