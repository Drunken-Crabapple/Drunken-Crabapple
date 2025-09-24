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

/*输出偏移
给输入加个偏移,直接跳过输出值较小的这个阶段,避免当输入较小的时候带不动执行器

若输出值为零,则正常输出零,若输出非零，则给输出值加一个固定偏移，跳过执行器无动作的阶段
输出为正:加上偏移
输出为负:减去偏移

输出偏移放在得到Out后，限幅前，得现有Out才能判断Out是否为零

偏移值 拿按键来看  按一下占空比加一，看加到几开始转，偏移量就是多少
*/


/*输入死区
加上输出偏移后会导致执行器在误差很小的时候频繁调控 不能稳定下来

若误差绝对值小于一个限度，则固定输出为零，不进行调控

*/


float Target, Actual, Out;
float Kp = 0.38, Ki =  0, Kd = 0.175;
float Error0,Error1,Error_sum;
	


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
			
			//判断死区，当误差位于死区区间时，直接给零，不调控
			if(fabs(Error0) < 5)
			{
				Out = 0;
			}
			else
			{
				if(fabs(Ki) > EPSILON)  //避免当Ki = 0时 Error_sum持续累加,然后当Ki不为零时猛的进行调控
				{
					Error_sum = Error_sum + Error0;
				}				
				
				else
				{
					Error_sum = 0;
				}
				
				Out = Kp * Error0 + Ki * Error_sum + Kd * (Error0 - Error1);
				
				if(Out > 0.1)
				{
					Out = Out + 5;
				}
				else if(Out < -0.1)
				{
					Out = Out - 5;
				}
				else
				{
					Out = 0;
				}
			}
			
			if(Out > 300) {Out = 300;}
			if(Out < -300) {Out = -300;}
			
			SetPWMA(-Out);
			
		}
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		
	}
}
