#include "stm32f10x.h"                  // Device header



void Timer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  //开启时钟
	
	TIM_InternalClockConfig(TIM1);						 //选择时基单元的时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;  //高级定时器用  此处不用 直接给0
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);  	//初始化时基单元
	
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);					//避免上电立刻进入中断开始计数
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);         //NVICy优先级分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;			//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM1,ENABLE);
	
	
}



