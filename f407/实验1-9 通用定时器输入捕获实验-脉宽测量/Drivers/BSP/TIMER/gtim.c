#include "./BSP/TIMER/gtim.h"

TIM_HandleTypeDef g_timx_icap_chy_handle;

void gtim_tim5_icap_ch1_init(uint16_t psc,uint16_t arr)
{
    TIM_IC_InitTypeDef timx_icap_chy = {0};
    
    g_timx_icap_chy_handle.Instance = TIM5;
    g_timx_icap_chy_handle.Init.Prescaler = psc;
    g_timx_icap_chy_handle.Init.Period = arr;
    g_timx_icap_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_IC_Init(&g_timx_icap_chy_handle);
    
    timx_icap_chy.ICPolarity = TIM_ICPOLARITY_RISING;           //上升沿捕获
    timx_icap_chy.ICSelection = TIM_ICSELECTION_DIRECTTI;       //映射到TI1上
    timx_icap_chy.ICPrescaler = TIM_ICPSC_DIV1;                 //配置输入分频
    timx_icap_chy.ICFilter = 0;                                 //配置输入滤波器
    
    HAL_TIM_IC_ConfigChannel(&g_timx_icap_chy_handle,&timx_icap_chy,TIM_CHANNEL_1); //配置输入通道映射 捕获边沿
    
    __HAL_TIM_ENABLE_IT(&g_timx_icap_chy_handle,TIM_IT_UPDATE); //使能定时器溢出中断
    HAL_TIM_IC_Start_IT(&g_timx_icap_chy_handle,TIM_CHANNEL_1); //使能捕获、捕获中断及计数器
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init_struct;
    if(htim->Instance == TIM5)
    {
        __HAL_RCC_TIM5_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        gpio_init_struct.Pin = GPIO_PIN_0;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLDOWN;
        gpio_init_struct.Alternate = GPIO_AF2_TIM5;
         
        HAL_GPIO_Init(GPIOA,&gpio_init_struct);
        
        HAL_NVIC_SetPriority(TIM5_IRQn,1,0);
        HAL_NVIC_EnableIRQ(TIM5_IRQn);
    }
}


/*输入捕获状态(g_timxchy_cap_sta)
    [7]  :是否完整捕获一次脉冲
    [6]  :是否捕捉到上升沿
    [5:0]:捕获到上升沿后计数器清零,即从捕获到第一次上升沿后开始计数,该六位代表捕获到上升沿
后溢出的次数,最多溢出63次。最长捕获值 = 63 * 65536 + 65535 = 4194303，约4.19秒
*/

uint8_t g_timxchy_cap_sta = 0;      //输入捕获状态
uint16_t g_timxchy_cap_val = 0;     //输入捕获值


void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_icap_chy_handle); //定时器HAL库公共处理函数
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM5)
    {
        if((g_timxchy_cap_sta & 0x80) == 0)     //第七位为零,说明还没有捕获到一次完整的脉冲,但只要进入到该回调函数，说明监测到一次上升沿(后续更改后为下降沿)
        {
            if(g_timxchy_cap_sta & 0x40)        //第六位为1，说明已经捕获到上升沿，这一次进来是因为捕获到下降沿
            {
                g_timxchy_cap_sta |= 0x80;      //将第七位置一，说明已经捕获到一次完整脉冲
                g_timxchy_cap_val = HAL_TIM_ReadCapturedValue(&g_timx_icap_chy_handle,TIM_CHANNEL_1);   //获取当前捕获值
                TIM_RESET_CAPTUREPOLARITY(&g_timx_icap_chy_handle,TIM_CHANNEL_1);        //清除原先设置，并改为上升沿监测，准备监测下一次完整的脉冲
                TIM_SET_CAPTUREPOLARITY(&g_timx_icap_chy_handle,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);
            }
            else        //  第六位为零，说明此次进入是因为捕获到了上升沿，此处清零计数器，以捕获到上升沿作为起点开始计数并修改为下降沿触发
            {
                g_timxchy_cap_sta = 0;
                g_timxchy_cap_val = 0;
                
                g_timxchy_cap_sta |= 0x40;      //第六位置一，捕获到上升沿
                __HAL_TIM_SET_COUNTER(&g_timx_icap_chy_handle,0);
                TIM_RESET_CAPTUREPOLARITY(&g_timx_icap_chy_handle,TIM_CHANNEL_1);
                TIM_SET_CAPTUREPOLARITY(&g_timx_icap_chy_handle,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);
            }
        }
    }
}

/*定时器溢出中断回调函数*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)      //只有当检测到上升沿后会进行超时处理，如果没有检测到上升沿，这个中断不会进行任何处理，直到上升沿到来
{
    if(htim->Instance == TIM5)
    {
        if((g_timxchy_cap_sta & 0x80) == 0)     //说明还未完整捕获一次脉冲，里面判断是否在63次溢出内都未完整捕获，如果是返回特殊值，说明超时
        {
            if(g_timxchy_cap_sta & 0x40)
            {
                if((g_timxchy_cap_sta & 0x3F) == 0x3F)      //说明63次溢出里面一直处于高电平，返回特殊值重新开始
                {
                    TIM_RESET_CAPTUREPOLARITY(&g_timx_icap_chy_handle,TIM_CHANNEL_1);
                    TIM_SET_CAPTUREPOLARITY(&g_timx_icap_chy_handle,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);
                    g_timxchy_cap_sta |= 0x80;      //任然标记成功捕获一次
                    g_timxchy_cap_val = 0xFFFF;     //返回特殊值
                }
                else        //溢出次数加一
                {
                    g_timxchy_cap_sta ++;
                }
                    
            }
        }
    }
}
