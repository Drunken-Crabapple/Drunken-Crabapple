#include "./BSP/TIMER/atim.h"

TIM_HandleTypeDef g_tim8_npwm_ch1_handle;
static uint8_t g_npwm_remain;       //不允许直接修改，尽量通过函数修改数据

void atim_tim8_npwm_ch1_init(uint16_t psc,uint16_t arr)
{
    TIM_OC_InitTypeDef timx_oc_npwm_chy = {0};
    
    g_tim8_npwm_ch1_handle.Instance = TIM8;
    g_tim8_npwm_ch1_handle.Init.Prescaler = psc;
    g_tim8_npwm_ch1_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim8_npwm_ch1_handle.Init.Period = arr;
    g_tim8_npwm_ch1_handle.Init.RepetitionCounter = 0;  //高级定时器专属：重复计数器
    HAL_TIM_PWM_Init(&g_tim8_npwm_ch1_handle);
    
    timx_oc_npwm_chy.OCMode = TIM_OCMODE_PWM2;
    timx_oc_npwm_chy.OCPolarity = TIM_OCPOLARITY_LOW;
    timx_oc_npwm_chy.Pulse = arr / 2;
    HAL_TIM_PWM_ConfigChannel(&g_tim8_npwm_ch1_handle,&timx_oc_npwm_chy,TIM_CHANNEL_1);
    
    __HAL_TIM_ENABLE_IT(&g_tim8_npwm_ch1_handle,TIM_IT_UPDATE);     //开启溢出中断
    HAL_TIM_PWM_Start(&g_tim8_npwm_ch1_handle,TIM_CHANNEL_1);       //启动PWM输出 使能主输出 计数器
    
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init_struct;
    if(htim->Instance == TIM8)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_TIM8_CLK_ENABLE();
        
        gpio_init_struct.Pin = GPIO_PIN_6;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = GPIO_AF3_TIM8;
        
        HAL_GPIO_Init(GPIOC,&gpio_init_struct);
        
        HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn,2,0);
        HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
    }
}
/*软件更新
        产生一次Updata事件
        重新装载影子寄存器
*/
/*人为设置npwm，使g_npwm_remain不再为零，此时第一次软件更新，会进入Updata中断，这一次会设置RCR寄存器
第二次软件更新会刷新影子寄存器，此时想要再次产生Update中断只能等重复寄存器减到零，所以第二次软件更新不会再次重头进入Update中断。
*/

void atim_npwm_chy_set(uint8_t npwm)
{
    if(npwm == 0)   
        return;
    g_npwm_remain = npwm;
    HAL_TIM_GenerateEvent(&g_tim8_npwm_ch1_handle,TIM_EVENTSOURCE_UPDATE);  //软件产生更新事件
    //因为前面初始化已经把UPDATE中断开启，此处软件产生更新事件后会立刻进入溢出回调函数
    __HAL_TIM_ENABLE(&g_tim8_npwm_ch1_handle);      //开启计数器
    
}

void TIM8_UP_TIM13_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim8_npwm_ch1_handle);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM8)
    {
        if(g_npwm_remain)       //人为设置过npwm，
        {
            TIM8->RCR = g_npwm_remain - 1;
            HAL_TIM_GenerateEvent(&g_tim8_npwm_ch1_handle,TIM_EVENTSOURCE_UPDATE);  //软件产生更新事件,将其存入影子寄存器
            __HAL_TIM_ENABLE(&g_tim8_npwm_ch1_handle);
            g_npwm_remain = 0;
        }
        else
        {
            TIM8->CR1 &= ~(1 << 0);     //关闭计数器
        }
    }
}



