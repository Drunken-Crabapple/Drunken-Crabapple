#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/TIMER/atim.h"
#include "./BSP/TIMER/gtim.h"


 extern TIM_HandleTypeDef g_tim2_cnt_ch1_handle;

int main(void)
{
    uint8_t curcnt = 0;
    uint8_t oldcnt = 0;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    atim_tim8_npwm_ch1_init(16800,10000);
    atim_npwm_chy_set(5);
    gtim_tim2_ch1_init(0);
    usart_init(115200);
    
    while(1)
    {
        curcnt = __HAL_TIM_GetCounter(&g_tim2_cnt_ch1_handle);
        if(oldcnt != curcnt)
        {
            atim_npwm_chy_set(5);
            oldcnt = curcnt;
            printf("您已按下 %d 次按键\r\n",curcnt);
        }
    }
}

