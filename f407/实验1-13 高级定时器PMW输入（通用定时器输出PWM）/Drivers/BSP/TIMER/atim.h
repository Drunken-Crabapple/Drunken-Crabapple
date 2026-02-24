#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"
extern TIM_HandleTypeDef g_timx_pwmin_chy_handle;
extern uint8_t g_timxchy_pwmin_sta;
extern uint16_t g_timxchy_pwmin_psc;
extern uint32_t g_timxchy_pwmin_hval;
extern uint32_t g_timxchy_pwmin_cval;

void atim_tim8_pwmin_ch1_init(void);

#endif
