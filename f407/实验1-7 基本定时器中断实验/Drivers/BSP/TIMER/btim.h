#ifndef __BTIM_H
#define __BTIM_H

#include "./SYSTEM/sys/sys.h"

extern TIM_HandleTypeDef g_timx_handle;
void btim_tim6_int_init(uint16_t psc,uint16_t arr);


#endif
