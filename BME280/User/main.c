#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/BME280/bme280.h"
#include "./BSP/TIMER/btim.h"

BME280_RawData raw;
BME280_CalibData calib;
BME280_Data data;

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    usart_init(115200);
    if(BME280_Init(&calib))
    {
        printf("error\r\n");
        while(1)
        {
        }
        
    }
    btim_tim6_cnt_init();
    
    while(1)
    {
        if(tim_50ms_flag)
        {
            tim_50ms_flag = 0;
            BME280_GetData(&raw,&calib,&data);
        }
        if(tim_500ms_flag)
        {
            tim_500ms_flag = 0;
            printf("%f,%f,%f\r\n",data.temp,data.press / 100.0,data.hum);
        }
        
    }
}

