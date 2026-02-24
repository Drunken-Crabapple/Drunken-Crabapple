#ifndef __IIC_H
#define __IIC_H

#include "SYSTEM/sys/sys.h"

#define IIC_SCL(x)      do{ x ? \
                            HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET) : \
                            HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET); \
                        }while (0)

#define IIC_SDA(x)      do{ x ? \
                            HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET) : \
                            HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); \
                        }while (0)
                        
#define IIC_READ_SDA    HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)

void iic_init(void);
void iic_start(void);
void iic_stop(void);
uint8_t iic_wait_ack(void);
void iic_ack(void);
void iic_nack(void);
void iic_send_byte(uint8_t data);
uint8_t iic_read_byte(uint8_t ack);


#endif
