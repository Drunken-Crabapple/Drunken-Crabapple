#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/TIMER/gtim.h"
#include "./BSP/TIMER/atim.h"


int main(void)
{
    extern TIM_HandleTypeDef g_timx_pwmin_chy_handle;
    extern uint8_t g_timxchy_pwmin_sta;
    extern uint16_t g_timxchy_pwmin_psc;
    extern uint32_t g_timxchy_pwmin_hval;
    extern uint32_t g_timxchy_pwmin_cval;

    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    gtim_tim3_pwm_ch2_init(84 - 1,10 - 1);

    
    
    while(1)
    {

    }
}

