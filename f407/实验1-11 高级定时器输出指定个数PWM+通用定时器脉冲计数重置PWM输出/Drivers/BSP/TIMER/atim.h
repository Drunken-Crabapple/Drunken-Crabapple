#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"

extern TIM_HandleTypeDef g_tim8_npwm_ch1_handle;
void atim_tim8_npwm_ch1_init(uint16_t psc,uint16_t arr);
void atim_npwm_chy_set(uint8_t npwm);


#endif
