#ifndef __ICM_H
#define __ICM_H

#include "./SYSTEM/sys/sys.h"
#define ICM_CS(x)       do{ x ? \
                            HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_SET) : \
                            HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_RESET); \
                        }while (0)
uint8_t SPI2_ReadWriteBytes(uint8_t addr,uint8_t ptxdata,uint8_t *prxdata,uint16_t datasize);
void icm_cs_init(void);
uint8_t ICM_ReadID(void);
void icm_init(void);
void icm_getdata(float *ax,float *ay,float *az,
                 float *gx,float *gy,float *gz);


#endif
