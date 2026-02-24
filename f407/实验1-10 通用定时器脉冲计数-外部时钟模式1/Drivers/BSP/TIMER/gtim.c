#include "./BSP/TIMER/gtim.h"

TIM_HandleTypeDef g_timx_cnt_chy_handle;

void gtim_tim2_ch1_init(uint16_t psc)
{
    TIM_SlaveConfigTypeDef tim_slave_config = {0};
    
    g_timx_cnt_chy_handle.Instance = TIM2;
    g_timx_cnt_chy_handle.Init.Prescaler = psc;
    g_timx_cnt_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_cnt_chy_handle.Init.Period = 65535;
    HAL_TIM_IC_Init(&g_timx_cnt_chy_handle);
    
    tim_slave_config.SlaveMode = TIM_SLAVEMODE_EXTERNAL1; //外部时钟模式1
    tim_slave_config.InputTrigger = TIM_TS_TI1FP1;
    tim_slave_config.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
    tim_slave_config.TriggerFilter = 8;
    HAL_TIM_SlaveConfigSynchro(&g_timx_cnt_chy_handle,&tim_slave_config);
    
    TIM_IC_InitTypeDef tim_ic_init = {0};
    tim_ic_init.ICFilter = 8;
    tim_ic_init.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    tim_ic_init.ICSelection = TIM_ICSELECTION_DIRECTTI;
    tim_ic_init.ICPrescaler = TIM_ICPSC_DIV1;
    HAL_TIM_IC_ConfigChannel(&g_timx_cnt_chy_handle,&tim_ic_init,TIM_CHANNEL_1);
    
    HAL_TIM_IC_Start(&g_timx_cnt_chy_handle,TIM_CHANNEL_1); //不带IT，不开启中断，检测到上升沿后会自动增加CNT，不用开启中断，此处只启动输入捕获
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        GPIO_InitTypeDef gpio_init_struct;
        
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM2_CLK_ENABLE();
        
        gpio_init_struct.Pin = GPIO_PIN_0;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLDOWN;
        gpio_init_struct.Alternate = GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOA,&gpio_init_struct);
    }

}


