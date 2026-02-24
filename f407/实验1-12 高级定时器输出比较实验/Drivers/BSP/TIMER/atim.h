#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"

extern TIM_HandleTypeDef g_timx_comp_pwm_handle;
void atim_tim8_comp_pwm_init(uint16_t psc,uint16_t arr);

#endif
