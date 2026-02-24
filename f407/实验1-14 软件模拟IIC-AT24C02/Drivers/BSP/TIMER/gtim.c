#include "./BSP/TIMER/gtim.h"

TIM_HandleTypeDef g_timx_cnt_chy_handle;

void gtim_tim5_cnt_ch1_init(uint16_t psc)
{
    g_timx_cnt_chy_handle.Instance = TIM5;
    g_timx_cnt_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_cnt_chy_handle.Init.Prescaler = psc;
    g_timx_cnt_chy_handle.Init.Period = 65535;
    HAL_TIM_IC_Init(&g_timx_cnt_chy_handle);

    TIM_SlaveConfigTypeDef timx_slave_cnt;
    timx_slave_cnt.InputTrigger = TIM_TS_TI1FP1;
    timx_slave_cnt.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
    timx_slave_cnt.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
    timx_slave_cnt.TriggerFilter = 8;
    HAL_TIM_SlaveConfigSynchro(&g_timx_cnt_chy_handle,&timx_slave_cnt);

    TIM_IC_InitTypeDef timx_ic_chy;
    timx_ic_chy.ICSelection = TIM_ICSELECTION_DIRECTTI;
    timx_ic_chy.ICPolarity = TIM_ICPOLARITY_RISING;
    timx_ic_chy.ICPrescaler = TIM_ICPSC_DIV1;
    timx_ic_chy.ICFilter = 8;
    HAL_TIM_IC_ConfigChannel(&g_timx_cnt_chy_handle,&timx_ic_chy,TIM_CHANNEL_1);

    HAL_TIM_IC_Start(&g_timx_cnt_chy_handle,TIM_CHANNEL_1);
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM5)
    {
        GPIO_InitTypeDef gpio_init_struct;
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM5_CLK_ENABLE();

        gpio_init_struct.Pin = GPIO_PIN_0;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLDOWN;
        gpio_init_struct.Alternate = GPIO_AF2_TIM5;

        HAL_GPIO_Init(GPIOA,&gpio_init_struct);
    }
}

