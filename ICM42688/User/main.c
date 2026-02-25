#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ICM/icm.h"
#include "./BSP/Mahony/Mahony.h"
#include "./BSP/TIMER/btim.h"

int main(void)
{

    extern float ax;
    extern float ay;
    extern float az;
    extern float gx;
    extern float gy;
    extern float gz;
    extern float roll;
    extern float yaw;
    extern float pitch;
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    usart_init(115200);
    icm_init();
    icm_getdata(&ax,&ay,&az,&gx,&gy,&gz);
    Mahony_Init(1000.0f);
    MahonyAHRSinit(ax,ay,az,0,0,0);   
    btim_tim6_cnt_init();

    while(1)
    {
        
    }
}

