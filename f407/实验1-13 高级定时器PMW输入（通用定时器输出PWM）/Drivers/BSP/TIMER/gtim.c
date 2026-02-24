#include "./BSP/TIMER/gtim.h"

TIM_HandleTypeDef g_timx_pwm_chy_handle;

void gtim_tim3_pwm_ch2_init(uint16_t psc,uint16_t arr)
{
    g_timx_pwm_chy_handle.Instance = TIM3;
    g_timx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_pwm_chy_handle.Init.Prescaler = psc;
    g_timx_pwm_chy_handle.Init.Period = arr;
    HAL_TIM_PWM_Init(&g_timx_pwm_chy_handle);

    TIM_OC_InitTypeDef timx_pwm_chy_oc;
    timx_pwm_chy_oc.OCMode = TIM_OCMODE_PWM2;
    timx_pwm_chy_oc.Pulse = arr / 2;
    timx_pwm_chy_oc.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(&g_timx_pwm_chy_handle,&timx_pwm_chy_oc,TIM_CHANNEL_2);

    HAL_TIM_PWM_Start(&g_timx_pwm_chy_handle,TIM_CHANNEL_2);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init_struct;
    if(htim->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        gpio_init_struct.Pin = GPIO_PIN_5;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = GPIO_AF2_TIM3;
        HAL_GPIO_Init(GPIOB,&gpio_init_struct);
    }
}

