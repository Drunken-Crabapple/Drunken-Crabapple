#include "stm32f10x.h"
#include "generic.h" //主要包含Delay
#include "OLED_I2C.h"
#include "MPU6050.h"
#include "Serial.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "Kalman_filter.h"
#include "Timer.h"

/*硬件连接:
OLED屏幕      SCL B10    SDA  B11
MPU6050       SCL A0     SDA  A1
USART(串口)	   TX  A9    RX  A10
*/

MPU6050 MM; // 实例化一个MPU6050对象
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // clear flag
        //MPU6050_Get_Angle_Plus(&MM);//获取角度
        MPU6050_Get_Angle(&MM); // 获取角度
    }
}

int main(void)
{
    Serial_Init(USART1, 115200);                 // 初始化串口
    //OLED_Init(GPIOB, GPIO_Pin_10, GPIO_Pin_11);  // 初始化OLED屏幕
    MPU6050_init(GPIOA, GPIO_Pin_0, GPIO_Pin_1); // 初始化MPU6050
    TImer_IT_init_ms(TIM3, 10);
	MPU6050_Set_Angle0(&MM);//让角度从0开始(只对MPU6050_Get_Angle_Plus有效)

    // mpu_dmp_init(GPIOA,GPIO_Pin_1,GPIO_Pin_0);//DMPPPP库
    while (1) {
        
        // mpu_dmp_get_data(&MM.pitch,&MM.roll,&MM.yaw);//DMPPPP库

        printf("%.3f,%.3f,%.3f\n", MM.roll, MM.pitch, MM.yaw); // 通过串口打印数据

		//以下为OLED屏幕
		// OLED_Clear();
        // OLED_Printf(0,0,6,"roll:%.2f",MM.roll);
        // OLED_Printf(0,8,6,"pitch:%.2f",MM.pitch);
        // OLED_Printf(0,16,6,"yaw:%.2f",MM.yaw);
        // OLED_Update();
    }
}
