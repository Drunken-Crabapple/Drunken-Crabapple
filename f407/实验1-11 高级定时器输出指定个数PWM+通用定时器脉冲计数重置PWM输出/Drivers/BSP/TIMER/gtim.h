#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"

extern TIM_HandleTypeDef g_tim2_cnt_ch1_handle;
void gtim_tim2_ch1_init(uint16_t psc);

#endif

