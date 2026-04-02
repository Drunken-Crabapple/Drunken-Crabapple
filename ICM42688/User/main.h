#ifndef __MAIN_H
#define __MAIN_H

#include "./SYSTEM/sys/sys.h"

typedef struct 
{
    int16_t x;
    int16_t y;
    int16_t z;
}Vector3i;

typedef struct 
{
    float x;
    float y;
    float z;
}Vector3f;

typedef struct 
{
    float roll;
    float pitch;
    float yaw;
}_out_angle;

typedef struct
{
    Vector3i offset_acc;
    Vector3i offset_gyro;
    Vector3i offset_mag;
    float hys;
    float hzs;
    int16_t data_x; 
}conf_t;


extern Vector3i raw_acc,raw_gyro;
extern Vector3i acc,gyro;
extern Vector3f out_acc,out_gyro;
extern Vector3f gyro_rad,gyro_deg;
extern conf_t conf;
extern _out_angle out_angle;

#endif
