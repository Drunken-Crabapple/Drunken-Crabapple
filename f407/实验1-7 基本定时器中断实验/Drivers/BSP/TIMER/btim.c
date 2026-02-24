#include "./BSP/TIMER/btim.h"


TIM_HandleTypeDef g_timx_handle;


/* 配置定时器基础工作参数 */
void btim_tim6_int_init(uint16_t psc,uint16_t arr)
{
    g_timx_handle.Instance = TIM6;
    g_timx_handle.Init.Prescaler = psc;         /* 分频 */
    g_timx_handle.Init.Period = arr;            /* 自动装载值 */
    g_timx_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    
    HAL_TIM_Base_Init(&g_timx_handle);
    
    HAL_TIM_Base_Start_IT(&g_timx_handle);      /* 使能定时器x和定时器更新中断  */
}

/* 定时器基础MSP初始化 配置CLOCK NVIC(基本定时器是纯内部基本定时器,没有任何外部引脚,通用和高级才有) */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        __HAL_RCC_TIM6_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM6_DAC_IRQn,2,1);
        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
    }
}

void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_handle);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_2);
    }
}
