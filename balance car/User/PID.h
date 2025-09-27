#ifndef __PID_H
#define __PID_H

typedef struct{
	float Target;
	float Actual;
	float Out;
	
	float Kp;
	float Ki;
	float Kd;
	
	float Error0;
	float Error1;
	float Error_sum;
	
	float OutMax;
	float OutMin;
} PID_t;

void PID_Update(PID_t *p);

#endif
