#include "./BSP/TIMER/atim.h"

TIM_HandleTypeDef g_timx_comp_pwm_handle;

void atim_tim8_comp_pwm_init(uint16_t psc,uint16_t arr)
{
    TIM_OC_InitTypeDef timx_oc_comp_pwm = {0};
 
    g_timx_comp_pwm_handle.Instance = TIM8;
    g_timx_comp_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;   //  递增计数
    g_timx_comp_pwm_handle.Init.Prescaler = psc;
    g_timx_comp_pwm_handle.Init.Period = arr;
    HAL_TIM_OC_Init(&g_timx_comp_pwm_handle);

    timx_oc_comp_pwm.OCMode = TIM_OCMODE_TOGGLE;        //输出比较里面的翻转模式，当计数值 = 比较值时，翻转电平(另一种PWM)，通过设置比较值设置占空比，两次为一个周期
    timx_oc_comp_pwm.OCPolarity = TIM_OCPOLARITY_HIGH;  ////因为像自由控制占空比，所以通过__HAL_TIM_SET_COMPARE();设置
    
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle,&timx_oc_comp_pwm,TIM_CHANNEL_1);
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle,&timx_oc_comp_pwm,TIM_CHANNEL_2);
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle,&timx_oc_comp_pwm,TIM_CHANNEL_3);
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle,&timx_oc_comp_pwm,TIM_CHANNEL_4);

    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle,TIM_CHANNEL_1);     //开启定时器通道的输出比较预装载，只有在产生更新事件后比较值才会更新带影子寄存器
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle,TIM_CHANNEL_2);
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle,TIM_CHANNEL_3);
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle,TIM_CHANNEL_4);   
    
    HAL_TIM_OC_Start(&g_timx_comp_pwm_handle,TIM_CHANNEL_1);                //开始输出比较
    HAL_TIM_OC_Start(&g_timx_comp_pwm_handle,TIM_CHANNEL_2);
    HAL_TIM_OC_Start(&g_timx_comp_pwm_handle,TIM_CHANNEL_3);
    HAL_TIM_OC_Start(&g_timx_comp_pwm_handle,TIM_CHANNEL_4);
}


void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM8)
    {
        GPIO_InitTypeDef gpio_init_struct;

        __HAL_RCC_TIM8_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pin = GPIO_PIN_6;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOC,&gpio_init_struct);

        gpio_init_struct.Pin = GPIO_PIN_7;
        gpio_init_struct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOC,&gpio_init_struct);

        gpio_init_struct.Pin = GPIO_PIN_8;
        gpio_init_struct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOC,&gpio_init_struct);

        gpio_init_struct.Pin = GPIO_PIN_9;
        gpio_init_struct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOC,&gpio_init_struct);

    }
}
