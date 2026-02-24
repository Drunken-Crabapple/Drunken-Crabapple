#include "./BSP/TIMER/gtim.h"

TIM_HandleTypeDef g_timx_pwm_chy_handle;

void gtim_tim10_pwm_ch1_init(uint16_t psc,uint16_t arr)
{
    TIM_OC_InitTypeDef timx_pwm_chy_oc;
    
    g_timx_pwm_chy_handle.Instance = TIM10;
    g_timx_pwm_chy_handle.Init.Prescaler = psc;
    g_timx_pwm_chy_handle.Init.Period = arr;
    g_timx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    
    timx_pwm_chy_oc.OCMode = TIM_OCMODE_PWM1;
    timx_pwm_chy_oc.Pulse = arr;        //比较值
    timx_pwm_chy_oc.OCPolarity = TIM_OCPOLARITY_LOW;  //低电平有效
    
    HAL_TIM_PWM_Init(&g_timx_pwm_chy_handle);
    HAL_TIM_PWM_ConfigChannel(&g_timx_pwm_chy_handle,&timx_pwm_chy_oc,TIM_CHANNEL_1);
    //ConfigChannel依赖定时器已经初始化完成，所以定时器初始化必须在该步骤前
    
    HAL_TIM_PWM_Start(&g_timx_pwm_chy_handle,TIM_CHANNEL_1);        //使能输出并启动计数器
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM10)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_TIM10_CLK_ENABLE();
        
        GPIO_InitTypeDef gpio_init_struct;
        
        gpio_init_struct.Pin = GPIO_PIN_8;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = GPIO_AF3_TIM10;
        
        HAL_GPIO_Init(GPIOB,&gpio_init_struct);
    }
}
