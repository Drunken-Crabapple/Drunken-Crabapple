#include "stm32f10x.h"    // Device header
#include "OLED.h"
#include "Delay.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "Motor.h"
#include "Encoder.h"
#include "Serial.h"
#include "math.h"

#define EPSILON 0.0001


/*编码器测试*/

float Target, Actual, Out;
float Kp = 0.38, Ki = 0.4, Kd = 0.175;
float Error0,Error1,Error_sum;
	


int main()
{
	Motor_Init();
	PWM_Init();
	OLED_Init();
	Encoder_Init();
	Serial_Init();
	
	Timer_Init();
	
	OLED_Printf(0, 0, OLED_8X16, "Speed Control");
	OLED_Update();
	
	Target = 250;
	
	while(1)
	{
		
		
		OLED_Printf(0,16,OLED_8X16,"Tar:%+04.0f",Target);//%f:显示浮点数 ，+表示始终显示正负 ，4表示显示宽度为4，0表示数值前面补0，  .0表示浮点数保留0位小数
		OLED_Printf(0,32,OLED_8X16,"Act:%+04.0f",Actual);
		OLED_Printf(0,48,OLED_8X16,"Out:%+04.0f",Out);
		
		OLED_Update();    
		Serial_Printf("%d,%d,%d",Actual,Target,Out);

		
	}
}

void TIM1_UP_IRQHandler(void)
{
	
	static uint16_t Count;
	
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		Count ++; 
		if(Count >= 40)  //40ms执行一次  Speed并不是速度  是40ms内编码器的脉冲数
		{
			Count = 0;
			
			Actual = Encoder1_Get();
			
			Error1 = Error0;
			Error0 = Target - Actual;
			
			if(Error_sum > 500) {Error_sum = 500; }
			if(Error_sum < -500) {Error_sum = -500;}
			
			Out = Kp * Error0 + Ki * Error_sum + Kd * (Error0 - Error1);
			
			if(Out > 300) {Out = 300;}
			if(Out < -300) {Out = -300;}
			
			SetPWMA(-Out);
			
		}
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		
	}
}
