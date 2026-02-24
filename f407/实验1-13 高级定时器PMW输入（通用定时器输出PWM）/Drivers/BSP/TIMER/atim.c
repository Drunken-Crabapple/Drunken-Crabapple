#include "./BSP/TIMER/atim.h"

TIM_HandleTypeDef g_timx_pwmin_chy_handle;

uint8_t g_timxchy_pwmin_sta = 0;    //pwm输入状态
uint16_t g_timxchy_pwmin_psc = 0;   //pwm输入分频系数
uint32_t g_timxchy_pwmin_hval = 0;  //pwm的高电平脉宽
uint32_t g_timxchy_pwmin_cval = 0;  //pwm的周期宽度

void atim_tim8_pwmin_ch1_init(void)
{
    g_timx_pwmin_chy_handle.Instance = TIM8;
    g_timx_pwmin_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_pwmin_chy_handle.Init.Prescaler = 0;
    g_timx_pwmin_chy_handle.Init.Period = 65535;
    HAL_TIM_IC_Init(&g_timx_pwmin_chy_handle);

    TIM_IC_InitTypeDef timx_pwmin_chy_ic = {0};
    timx_pwmin_chy_ic.ICSelection = TIM_ICSELECTION_DIRECTTI;//TI1FP1
    timx_pwmin_chy_ic.ICPolarity = TIM_ICPOLARITY_RISING;   //IC1上升沿
    timx_pwmin_chy_ic.ICPrescaler = TIM_ICPSC_DIV1;                      //不分频
    timx_pwmin_chy_ic.ICFilter = 0;                         //不滤波
    HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle,&timx_pwmin_chy_ic,TIM_CHANNEL_1);

    timx_pwmin_chy_ic.ICPolarity = TIM_ICPOLARITY_FALLING;          //IC2:下降沿
    timx_pwmin_chy_ic.ICSelection = TIM_ICSELECTION_INDIRECTTI;     //选择输入端IC2映射到TI1,也就是信号输入是在CH1，软件复制一份给CH2进行处理 即TI1FP2
    HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle,&timx_pwmin_chy_ic,TIM_CHANNEL_2);

    TIM_SlaveConfigTypeDef slave_config = {0};
    slave_config.SlaveMode = TIM_SLAVEMODE_RESET;       //从模式：复位模式
    slave_config.InputTrigger = TIM_TS_TI1FP1;          //定时器输入触发源：TI1FP1
    slave_config.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;//上升沿检测
    slave_config.TriggerFilter = 0;
    HAL_TIM_SlaveConfigSynchro(&g_timx_pwmin_chy_handle,&slave_config);  

    HAL_TIM_IC_Start_IT(&g_timx_pwmin_chy_handle,TIM_CHANNEL_1);        //开启通道一的中断，但通道二不开，因为物理意义上的信号只从通道一进入
    HAL_TIM_IC_Start(&g_timx_pwmin_chy_handle,TIM_CHANNEL_2);

}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init_struct;
    if(htim->Instance == TIM8)
    {
        __HAL_RCC_TIM8_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        gpio_init_struct.Pin = GPIO_PIN_6;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLDOWN;
        gpio_init_struct.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOC,&gpio_init_struct);

        HAL_NVIC_SetPriority(TIM8_CC_IRQn,1,3);
        HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
    }
}

void TIM8_CC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_pwmin_chy_handle);   //定时器公共处理函数
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM8)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            g_timxchy_pwmin_hval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_chy_handle,TIM_CHANNEL_1) + 1 + 1;
            g_timxchy_pwmin_cval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_chy_handle,TIM_CHANNEL_2);
            g_timxchy_pwmin_sta = 1;    //标记成功捕获

        }
    }
}

void atim_timx_pwmin_chy_restart(void)
{
    sys_intx_disable();     //关闭中断

    g_timxchy_pwmin_sta = 0;    //清零状态，重新开始检测
    g_timxchy_pwmin_hval = 0;
    g_timxchy_pwmin_cval = 0;

    sys_intx_enable();          //打开中断
}
