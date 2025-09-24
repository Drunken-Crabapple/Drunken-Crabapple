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
#include "PID.h" 

#define EPSILON 0.0001

/*双环PID定速定位置控制  外环:速度环  内环:位置环
用户控制外环目标值,内环目标值由外环的输出值控制 
外环周期一般大于内环周期  反之 外环刷新再快，而内环读取很慢也没有意义 

先把外环PID的代码注释掉,手动控制内环目标值,不要让外环工作,调节内环参数,再在内环的基础上调节外环参数
内环0.3 0.3 0,然后把外环注释取消
*/
 

int16_t Speed,Location; 

PID_t Inner = {
	.Kp = 0.3,
	.Ki = 0.3,
	.Kd = 0,
	.OutMax = 100,
	.OutMin = -100,
};
	
PID_t Outer = 
{
	.Kp = 0.3,
	.Ki = 0,
	.Kd = 0.4,
	.OutMax = 150,
	.OutMin = -150,
};

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
	
	Outer.Target = 250;
	
	
	while(1)
	{
		
		
//		OLED_Printf(0,16,OLED_8X16,"Tar:%+04.0f",Inner.Target);   
//		OLED_Printf(0,32,OLED_8X16,"Act:%+04.0f",Inner.Actual);
//		OLED_Printf(0,48,OLED_8X16,"Out:%+04.0f",Inner.Out);
//		
//		OLED_Update();    
//		Serial_Printf("%d,%d,%d",Inner.Actual,Inner.Target,Inner.Out);

		OLED_Printf(0,16,OLED_8X16,"Tar:%+04.0f",Outer.Target);   
		OLED_Printf(0,32,OLED_8X16,"Act:%+04.0f",Outer.Actual);
		OLED_Printf(0,48,OLED_8X16,"Out:%+04.0f",Outer.Out);
		
		OLED_Update();    
		Serial_Printf("%d,%d,%d",Outer.Actual,Outer.Target,Outer.Out);
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
			
			Inner.Actual = Speed;
			
			PID_Update(&Inner);
			
			SetPWMA(-Inner.Out);
			
		}
		
		if(Count2 >= 40)     
		{
			Count2 = 0;
			
		//	OuterActual += Encoder1_Get();  //Encoder_Get每次调用后会清零计数器，所以整个PID计算里面只能调用一次Encoder_Get
								//重新定义变量一次储存两个
			Outer.Actual = Location;
			
			PID_Update(&Outer);
			
			//SetPWMA(-OuterOut);  外环输出值不能直接给电机，而是要作用于内环目标值
			
			Inner.Target = Outer.Out;//这两限幅要求一样
			
		}
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		
	}
}
