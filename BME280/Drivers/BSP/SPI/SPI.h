#ifndef __SPI_H
#define __SPI_H

#include "./SYSTEM/sys/sys.h"

extern SPI_HandleTypeDef g_spi2_handle;

void spi2_init(void);
uint8_t spi2_read_write_byte(uint8_t txdata);


#endif
