#ifndef __MOVE_H
#define __MOVE_H

#include "./SYSTEM/sys/sys.h"

extern float turn_delta;
extern float turn;
extern float base_target;

void motion_init(void);

void Direct_line(int16_t openmv_offset);
void Turn(int8_t TURN_FLAG);
void Turn_back(void);

void Car_Control_Init(uint8_t first_turn_cross_index,int8_t turn_flag);
void Car_Control_Update(uint8_t openmv_state,int16_t openmv_offset);

#endif
