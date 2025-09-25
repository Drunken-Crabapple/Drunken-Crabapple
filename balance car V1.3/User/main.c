#include "stm32f10x.h"
#include "generic.h" //含Delay
#include "OLED_I2C.h"
#include "MPU6050.h"
#include "Serial.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "Kalman_filter.h"
#include "Timer.h"
#include "Motor.h"
#include "Encoder.h"
#include "PID.h"

/*
OLED屏幕      SCL B8    SDA  B9
MPU6050       SCL B10     SDA  B11
USART	   TX  A9    RX  A10
*/

MPU6050 MM; 
float zhongzhi = 0;
float PWM ;

PID_t Vertical = {
	.Kp = -750,
	.Ki = 0,
	.Kd =-2000,
    .OutMax = 5000,
	.OutMin = -5000,
};

PID_t Velocity = {
	.Kp = 190,
	.Ki = 0.95,
	.Kd = 0,
	.OutMax = 5000,
	.OutMin = -5000,
};

int main(void)
{
	Encoder_Init();
	Motor_Init();
	
    Serial_Init(USART1, 115200);                
    OLED_Init(GPIOB, GPIO_Pin_8, GPIO_Pin_9);  
    MPU6050_init(GPIOB, GPIO_Pin_10, GPIO_Pin_11);
    TImer_IT_init_ms(10);
	
	
    while (1) {
        
 

  //     Serial_Printf("%.3f,%.3f,%.3f\n", MM.roll, MM.pitch, MM.yaw);
		//VOFA测试
		
		 OLED_Clear();
         OLED_Printf(0,0,6,"roll:%.2f",MM.roll);
         OLED_Printf(0,8,6,"pitch:%.2f",MM.pitch);
         OLED_Printf(0,16,6,"yaw:%.2f",MM.yaw);
         OLED_Update();
    }
}

void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // clear flag
        MPU6050_Get_Angle(&MM); 
		
		Vertical.Actual = MM.pitch;
		Vertical.Target = zhongzhi;
		
		Velocity.Actual = (Encoder1_Get() + Encoder2_Get()) / 2;
		Velocity.Target = zhongzhi;
		
		PWM = Velocity.Out + Vertical.Out;
		if(PWM > 6000) {PWM = 6000;}
		if(PWM < -6000) {PWM = -6000;}
		
		SetPWMA(PWM);
		SetPWMB(-PWM);
    }
}
