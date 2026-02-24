#include "./BSP/KEY/key.h"
#include "./SYSTEM/sys/sys.h"

void key_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    
    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_8;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLDOWN;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA,&gpio_init_struct);
    HAL_GPIO_Init(GPIOE,&gpio_init_struct);
}

uint8_t key_get(void)
{
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 1)
    {
        delay_ms(10);
        if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 1)
        {
            while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0));
            return 1;
        }
    }
    return 0;
}
