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

/*微分先行 V1.1
普通PID微分项对误差进行微分，当目标值大幅度跳变时，误差也会瞬间大幅度跳变，这会导致微分项突然输出一个很大的调控力
 


*/



int16_t Speed;
int16_t Location;
int16_t PWM = 100 ;

float Target, Actual, Out;
float Kp = 0.15, Ki = 0.004, Kd = 0.75;
float Error0,Error1,Error_sum;
float DifOut;
float Actual1;


int main()
{
	Motor_Init();
	PWM_Init();
	OLED_Init();
	Encoder_Init();
	Serial_Init();
	
	Timer_Init();
	
	Target = 550;
	
	while(1)
	{
		
		
		OLED_Printf(0,16,OLED_8X16,"Tar:%+04.0f",Target);
		OLED_Printf(0,32,OLED_8X16,"Act:%+04.0f",Actual);
		OLED_Printf(0,48,OLED_8X16,"Out:%+04.0f",Out);
		Delay_ms(200);
		
		OLED_Update();    
		Serial_Printf("%d,%d,%d,%f",Actual,Target,Out,DifOut);
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
			
			Actual1 = Actual;
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
			
			//DifOut = Kd * (Error0 - Error1);
			DifOut = -Kd * (Actual - Actual1);
			
			Out += Kp * Error0 + Ki * Error_sum + DifOut;
			
			if(Out > 300) {Out = 300;}
			if(Out < -300) {Out = -300;}
			
			SetPWMA(-Out);
			
		}
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		
	}
}
