#include "./BSP/EXTI/exti.h"
#include "./SYSTEM/delay/delay.h"

void exti_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.Pin = GPIO_PIN_0;
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;
    gpio_init_struct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA,&gpio_init_struct);
    
    HAL_NVIC_SetPriority(EXTI0_IRQn,2,0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler(void)  //中断服务函数
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);  //当EXTI0线产生中断后会调用该公共处理函数
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) //公共处理函数会调用这个函数，一调用说明按键按下
{
    delay_ms(10);
    if(GPIO_Pin == GPIO_PIN_0) //每一条线产生中断都会调用公共处理函数，然后调用这个callback函数，所以需要判断是哪一条exti线产生的中断
    {
        if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 1)
        {
            HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);
            while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0));
        }
        else
        {
            HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET);
        }
    }
    
}

