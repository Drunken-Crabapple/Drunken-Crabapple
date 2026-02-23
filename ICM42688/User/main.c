#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ICM/icm.h"

int main(void)
{
    float temp = 0;
    float ax = 0;
    float ay = 0;
    float az = 0;
    float gx = 0;
    float gy = 0;
    float gz = 0;
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    usart_init(115200);
    icm_init();


    while(1)
    {
        icm_getdata(&temp,&ax,&ay,&az,&gx,&gy,&gz);
        printf("%f,%f,%f,%f,%f,%f,%f\r\n",temp,ax,ay,az,gx,gy,gz);
    }
}

