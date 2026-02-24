#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/TIMER/btim.h"
#include "./BSP/LED/led.h"


void led_init(void);                    /* LED初始化函数声明 */

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336,8,2,7);
    delay_init(168);
    led1_init();
    btim_tim6_int_init(4200 - 1,5000 - 1);

    
    while(1)
    {
        
        
    }
}

