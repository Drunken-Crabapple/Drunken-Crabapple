#include "./BSP/LED/led.h"

void led_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.Pin = GPIO_PIN_2 | GPIO_PIN_8;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&gpio_init_struct);
    
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET);    //Ä¬ÈÏÃð
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);      //Ä¬ÈÏÃð
}
    
