#include "./SYSTEM/sys/sys.h"
#include "main.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ICM/icm.h"
#include "./BSP/TIMER/btim.h"
#include "./BSP/Imux/Imux.h"

Vector3i raw_acc,raw_gyro;
Vector3i acc,gyro;
Vector3f out_acc,out_gyro;
Vector3f gyro_rad,gyro_deg;
conf_t conf;
_out_angle out_angle;
extern uint8_t tim_1ms_flag;
extern uint8_t tim_2ms_flag;
extern uint8_t tim_100ms_flag;
float rr,pp,yy;
int16_t ax,ay,az,gx,gy,gz;
uint8_t key = 0;

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    usart_init(115200);
    icm_init();
    btim_tim6_cnt_init();
    Do_ACC_GYRO_Offset();
    delay_ms(1000);
    while(1)
    {
        if(key)
        {
            key = 0;
            Do_ACC_GYRO_Offset();
        }
        if(tim_1ms_flag)
        {
            tim_1ms_flag = 0;
            READ_ICM42688();
            sensor_data_prepare();
        }
        if(tim_2ms_flag)
        {
            tim_2ms_flag = 0;
            IMUupdate(gyro_rad.x,gyro_rad.y,gyro_rad.z,out_acc.x,out_acc.y,out_acc.z);
            Get_Eulerian_Angle(&out_angle);
        }
        if(tim_100ms_flag)
        {
            tim_100ms_flag = 0;
            ax = gyro.x;
            ay = gyro.y;
            az = gyro.z;
            gx = out_gyro.x;
            gy = out_gyro.y;
            gz = out_gyro.z;
            rr=out_angle.roll;
            pp=out_angle.pitch;
            yy=out_angle.yaw;
            printf("%5.1f,%5.1f,%5.1f\r\n",rr,pp,yy);
        }
    }
}

