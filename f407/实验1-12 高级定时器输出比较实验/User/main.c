#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/TIMER/atim.h"

/*
通过TIM8的 CH1 CH2 CH3 CH4输出相位分别为25% 50% 75% 100%的PWM波(设置比较值)
但占空比始终是50%，因为对于输出比较里面的翻转模式来说，两个溢出周期才是一个PWM周期，互补
*/


int main(void)
{
    extern TIM_HandleTypeDef g_timx_comp_pwm_handle;

    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    atim_tim8_comp_pwm_init(168,1000 - 1);  //溢出时间是0.001，输出周期是两倍，频率为500Hz

    __HAL_TIM_SET_COMPARE(&g_timx_comp_pwm_handle,TIM_CHANNEL_1,250 - 1);   //25%
    __HAL_TIM_SET_COMPARE(&g_timx_comp_pwm_handle,TIM_CHANNEL_2,500 - 1);   //50%
    __HAL_TIM_SET_COMPARE(&g_timx_comp_pwm_handle,TIM_CHANNEL_3,750 - 1);   //75% 
    __HAL_TIM_SET_COMPARE(&g_timx_comp_pwm_handle,TIM_CHANNEL_4,1000 - 1);  //100%

    while(1)
    {

    }
}

