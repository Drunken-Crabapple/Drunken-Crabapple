#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"
void gtim_tim5_icap_ch1_init(uint16_t psc,uint16_t arr);
extern TIM_HandleTypeDef g_timx_icap_chy_handle;
extern uint8_t g_timxchy_cap_sta; 
extern uint16_t g_timxchy_cap_val;



#endif
