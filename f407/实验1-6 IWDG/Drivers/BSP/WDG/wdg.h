#ifndef __WDG_H
#define __WDG_H

#include "./SYSTEM/sys/sys.h"

extern IWDG_HandleTypeDef g_iwdg_handle;

void iwdg_init(uint8_t pres,uint16_t rlr);
void iwdg_feed(void);


#endif
