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


/*±àÂëÆ÷²âÊÔ*/

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
	
	OLED_Printf(0, 0, OLED_8X16, "Location Control");
	OLED_Update();
	
	Target = 250;
	
	while(1)
	{
		
		
		OLED_Printf(0,16,OLED_8X16,"Tar:%+04.0f",Target);//%f:ÏÔÊ¾¸¡µãÊý £¬+±íÊ¾Ê¼ÖÕÏÔÊ¾Õý¸º £¬4±íÊ¾ÏÔÊ¾¿í¶ÈÎª4£¬0±íÊ¾ÊýÖµÇ°Ãæ²¹0£¬  .0±íÊ¾¸¡µãÊý±£Áô0Î»Ð¡Êý
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
		if(Count >= 40)  //40msÖ´ÐÐÒ»´Î  Speed²¢²»ÊÇËÙ¶È  ÊÇ40msÄÚ±àÂëÆ÷µÄÂö³åÊý
		{
			Count = 0;
			
			Actual += Encoder1_Get();
			
			Error1 = Error0;
			Error0 = Target - Actual;
			
			float C = 1 / (0.2 * fabs(Error0) + 1);
			//ç›¸å½“äºŽåœ¨å‰é¢ç§¯åˆ†åˆ†ç¦»åŸºç¡€ä¸Šæ”¹è¿›  å¹³æ»‘å¤„ç†ä¸´ç•Œç‚¹ è€Œä¸æ˜¯ä¸€å¤§äºŽè¯¯å·®å°±ç›´æŽ¥æ’¤èµ°ç§¯åˆ†é¡¹
			
			Error_sum = Error_sum + Error0;
			
			Out = Kp * Error0 + Ki * Error_sum + Kd * (Error0 - Error1);
			
			if(Out > 300) {Out = 300;}
			if(Out < -300) {Out = -300;}
			
			SetPWMA(-Out);
			
		}
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		
	}
}
