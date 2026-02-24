#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"

extern TIM_HandleTypeDef g_timx_pwm_chy_handle;

void gtim_tim10_pwm_ch1_init(uint16_t psc,uint16_t arr);

#endif
