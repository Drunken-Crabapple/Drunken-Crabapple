#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
void PWM_Init(void);
void PWM_SetCompare1(uint16_t Compare);
void PWM_SetCompare2(uint16_t Compare);
void SetPWMA(int PWM);
void SetPWMB(int PWM);


#endif
