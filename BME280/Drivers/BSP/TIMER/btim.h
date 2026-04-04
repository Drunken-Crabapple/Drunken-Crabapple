#ifndef __BTIM_H
#define __BTIM_H

#include "./SYSTEM/sys/sys.h"
void btim_tim6_cnt_init(void);

extern uint8_t tim_50ms_flag;
extern uint8_t tim_500ms_flag;

#endif
