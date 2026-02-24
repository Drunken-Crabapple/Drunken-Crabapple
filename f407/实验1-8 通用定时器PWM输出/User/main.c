#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/TIMER/gtim.h"



int main(void)
{
    extern TIM_HandleTypeDef g_timx_pwm_chy_handle;
    uint16_t flag = 1;
    uint16_t pwmval = 500;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    gtim_tim10_pwm_ch1_init(168 - 1,500 - 1);
    
    while(1)
    {
        delay_ms(5);
        if(flag)
            pwmval--;
        else
            pwmval++;
        if(pwmval <= 0)
            flag = 0;
        if(pwmval >= 500)
            flag = 1;
        __HAL_TIM_SET_COMPARE(&g_timx_pwm_chy_handle,TIM_CHANNEL_1,pwmval);
        
    }
}

