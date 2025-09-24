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

/*双环PID定速定位置控制  外环:速度环  内环:位置环
用户控制外环目标值,内环目标值由外环的输出值控制 
外环周期一般大于内环周期  反之 外环刷新再快，而内环读取很慢也没有意义 

先把外环PID的代码注释掉,手动控制内环目标值,不要让外环工作,调节内环参数,再在内环的基础上调节外环参数
内环0.3 0.3 0,然后把外环注释取消
*/
 

int16_t Speed,Location; 
 
 
float InnerTarget, InnerActual, InnerOut;
float InnerKp = 0.3, InnerKi = 0.3, InnerKd = 0;
float InnerError0,InnerError1,InnerError_sum;
	
float OuterTarget, OuterActual, OuterOut;
float OuterKp = 0.3, OuterKi = 0, OuterKd = 0.4 ;
float OuterError0,OuterError1,OuterError_sum;


int main()
{
	Motor_Init();
	PWM_Init();
	OLED_Init();
	Encoder_Init();
	Serial_Init();
	
	Timer_Init();
	
	OLED_Printf(0, 0, OLED_8X16, "2*PID Control");
	OLED_Update();
	
	OuterTarget = 250;
	
	
	while(1)
	{
		
		
//		OLED_Printf(0,16,OLED_8X16,"Tar:%+04.0f",InnerTarget);   
//		OLED_Printf(0,32,OLED_8X16,"Act:%+04.0f",InnerActual);
//		OLED_Printf(0,48,OLED_8X16,"Out:%+04.0f",InnerOut);
//		
//		OLED_Update();    
//		Serial_Printf("%d,%d,%d",InnerActual,InnerTarget,InnerOut);

		OLED_Printf(0,16,OLED_8X16,"Tar:%+04.0f",OuterTarget);   
		OLED_Printf(0,32,OLED_8X16,"Act:%+04.0f",OuterActual);
		OLED_Printf(0,48,OLED_8X16,"Out:%+04.0f",OuterOut);
		
		OLED_Update();    
		Serial_Printf("%d,%d,%d",OuterActual,OuterTarget,OuterOut);
	}
}

void TIM1_UP_IRQHandler(void)
{
	
	static uint16_t Count1,Count2;
	
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		Count1 ++; 
		if(Count1 >= 40)     //内环调控周期是40MS
		{
			Count1 = 0;
			
		//	InnerActual = Encoder1_Get();  注意嗷
			Speed = Encoder1_Get();
			Location += Speed;
			
			InnerActual = Speed;
			
			InnerError1 = InnerError0;
			InnerError0 = InnerTarget - InnerActual;
			
			if(fabs(InnerKi) > EPSILON)  //避免当Ki = 0时 Error_sum持续累加,然后当Ki不为零时猛的进行调控
			{
				InnerError_sum = InnerError_sum + InnerError0;
			}				
			
			else
			{
				InnerError_sum = 0;
			}
			
			InnerOut = InnerKp * InnerError0 + InnerKi * InnerError_sum + InnerKd * (InnerError0 - InnerError1);
			
			if(InnerOut > 300) {InnerOut = 300;}
			if(InnerOut < -300) {InnerOut = -300;}
			
			SetPWMA(-InnerOut);
			
		}
		
		if(Count2 >= 40)     
		{
			Count2 = 0;
			
		//	OuterActual += Encoder1_Get();  //Encoder_Get每次调用后会清零计数器，所以整个PID计算里面只能调用一次Encoder_Get
								//重新定义变量一次储存两个
			OuterActual = Location;
			
			OuterError1 = OuterError0;
			OuterError0 = OuterTarget - OuterActual;
			
			if(fabs(OuterKi) > EPSILON)  //避免当Ki = 0时 Error_sum持续累加,然后当Ki不为零时猛的进行调控
			{
				OuterError_sum = OuterError_sum + OuterError0;
			}				
			
			else
			{
				OuterError_sum = 0;
			}
			
			OuterOut = OuterKp * OuterError0 + OuterKi * OuterError_sum + OuterKd * (OuterError0 - OuterError1);
			
			if(OuterOut > 300) {OuterOut = 300;}
			if(OuterOut < -300) {OuterOut = -300;}
			
			//SetPWMA(-OuterOut);  外环输出值不能直接给电机，而是要作用于内环目标值
			
			InnerTarget = OuterOut;//这两限幅要求一样
			
		}
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		
	}
}
