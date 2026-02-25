#include "./BSP/TIMER/btim.h"
#include "./BSP/Mahony/Mahony.h"
#include "./BSP/ICM/icm.h"
#include "./SYSTEM/usart/usart.h"


TIM_HandleTypeDef g_tim6_cnt_handle;
float ax = 0;
float ay = 0;
float az = 0;
float gx = 0;
float gy = 0;
float gz = 0;
float roll = 0;
float yaw = 0;
float pitch = 0;

void btim_tim6_cnt_init(void)
{
    g_tim6_cnt_handle.Instance = TIM6;
    g_tim6_cnt_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim6_cnt_handle.Init.Prescaler = 8400 - 1;
    g_tim6_cnt_handle.Init.Period = 10 - 1;

    HAL_TIM_Base_Init(&g_tim6_cnt_handle);
    HAL_TIM_Base_Start_IT(&g_tim6_cnt_handle);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    
    if(htim->Instance == TIM6)
    {
        __HAL_RCC_TIM6_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM6_DAC_IRQn,2,0);
        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
    }
}

void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim6_cnt_handle);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        icm_getdata(&ax,&ay,&az,&gx,&gy,&gz);
        MahonyAHRSinit(ax,ay,az,0,0,0);

        Mahony_update(gx,gy,gz,ax,ay,az,0,0,0);
        Mahony_computeAngles();
        roll = getRoll();
        pitch = getPitch();
        yaw = getYaw();
        //printf("%f,%f,%f,%f,%f,%f,%f,%f,%f\r\n",ax,ay,az,gx,gy,gz,roll,pitch,yaw);
        printf("%f,%f,%f\r\n",roll,pitch,yaw);

    }
}
