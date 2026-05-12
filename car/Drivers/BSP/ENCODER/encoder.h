#ifndef __ENCODER_H
#define __ENCODER_H

#include "./SYSTEM/sys/sys.h"

typedef struct 
{
    uint16_t encoder_last;
    int16_t encoder_diff;
    float motor_rps;
    float motor_rpm;
}encoder_speed;

extern encoder_speed encoder1_speed;
extern encoder_speed encoder2_speed;

void encoder1_tim3_init(void);
void encoder2_tim4_init(void);
int16_t motor1_encoder_speed_update(void);
int16_t motor2_encoder_speed_update(void);

#endif

