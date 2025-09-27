#include "stm32f10x.h"                  // Device header



void PWM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM4);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM4,&TIM_OCInitStructure);
	TIM_OC2Init(TIM4,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM4,ENABLE);
	
}

void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM4,Compare);
}

void PWM_SetCompare2(uint16_t Compare)
{
	TIM_SetCompare2(TIM4,Compare);
}


void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	PWM_Init();
}

void Leftz(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}

void Leftf(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}

void Rightz(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
}

void Rightf(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
}

void SetPWMA(int PWM)
{
	if(PWM > 0)
	{
		Leftz();
		Rightz();
		PWM_SetCompare1(PWM);
//		PWM_SetCompare2(PWM);
	}
	else
	{
		Leftf();
		Rightf();
		PWM_SetCompare1(-PWM);
//		PWM_SetCompare2(-PWM);
	}
}

void SetPWMB(int PWM)
{
	if(PWM > 0)
	{
		Leftz();
		Rightz();
//		PWM_SetCompare1(PWM);
		PWM_SetCompare2(PWM);
	}
	else
	{
		Leftf();
		Rightf();
//		PWM_SetCompare1(-PWM);
		PWM_SetCompare2(-PWM);
	}
}
