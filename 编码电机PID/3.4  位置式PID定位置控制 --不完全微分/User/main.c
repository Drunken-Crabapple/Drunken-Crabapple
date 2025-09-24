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

/*不完全微分 V1.1(受噪声干扰的实际值通常对微分项影响最大)
给微分项加入一阶低通滤波  但可能会降低pid的处理速度 
*/

float Target, Actual, Out;
float Kp = 0.38, Ki = 0.4, Kd = 0.175;
float Error0,Error1,Error_sum;
float Difout;


int main()
{
	Motor_Init();
	PWM_Init();
	OLED_Init();
	Encoder_Init();
	Serial_Init();
	
	Timer_Init();
	
	OLED_Printf(0, 0, OLED_8X16, "Location Control");
	OLED_Update();
	
	Target = 250;
	
	while(1)
	{
		
		
		OLED_Printf(0,16,OLED_8X16,"Tar:%+04.0f",Target);//%f:��ʾ������ ��+��ʾʼ����ʾ���� ��4��ʾ��ʾ����Ϊ4��0��ʾ��ֵǰ�油0��  .0��ʾ����������0λС��
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
		if(Count >= 40)  
		{
			Count = 0;
			
			Actual += Encoder1_Get();
			
			Error1 = Error0;
			Error0 = Target - Actual;
			
			if(fabs(Ki) > EPSILON)  //避免当Ki = 0时 Error_sum持续累加,然后当Ki不为零时猛的进行调控
			{
				Error_sum = Error_sum + Error0;
			}				
			
			else
			{
				Error_sum = 0;
			}
			
			float a = 0.9;
			Difout = (1 - a) * Kd * (Error0 - Error1) + a * Difout;
			//此处两个Difout，后面那个是赋值前的的，相当于上一次的，赋值后的为此次的
			
			Out = Kp * Error0 + Ki * Error_sum + Difout;
			
			if(Out > 300) {Out = 300;}
			if(Out < -300) {Out = -300;}
			
			SetPWMA(-Out);
			
		}
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		
	}
}
