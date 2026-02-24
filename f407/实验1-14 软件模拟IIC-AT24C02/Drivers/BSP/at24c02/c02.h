#ifndef __C02_H
#define __C02_H

#include "./SYSTEM/sys/sys.h"

void at24c02_init(void);
uint8_t at24c02_receive_one_byte(uint8_t addr);
void at24c02_send_one_byte(uint8_t addr,uint8_t data);

#endif
