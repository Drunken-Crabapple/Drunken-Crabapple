#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/BME280/bme280.h"

BME280_RawData raw;
BME280_CalibData calib;
BME280_Data data;

float temp = 0.0f,press = 0.0f,hum = 0.0f;

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    usart_init(115200);
    if(BME280_Init(&calib))
    {
        printf("error/r/n");
    }
    
    while(1)
    {
        BME280_GetData(&raw,&calib,&data);
        temp = data.temp;
        press = data.press;
        hum = data.hum;
        printf("%f,%f,%f\r\n",temp,press,hum);
    }
}

