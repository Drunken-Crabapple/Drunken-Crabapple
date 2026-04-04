#ifndef __BME280_H
#define __BME280_H

#include "./SYSTEM/sys/sys.h"

#define BME_CS(x)       do{ x ? \
                            HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET) : \
                            HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET); \
                        }while (0)

typedef struct
{
    int32_t temp_raw;
    int32_t press_raw;
    int32_t hum_raw;
}BME280_RawData;

typedef struct 
{

    uint16_t dig_T1;    
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
}BME280_CalibData;

typedef struct 
{
    double temp;
    double press;
    double hum;
}BME280_Data;

uint8_t BME280_Init(BME280_CalibData *calib);
void BME280_GetData(BME280_RawData *raw, BME280_CalibData *calib,BME280_Data *data);


#endif
