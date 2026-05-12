#ifndef __MOVE_H
#define __MOVE_H

#include "./SYSTEM/sys/sys.h"

extern float turn_delta;
extern float turn;
extern float base_target;

void Direct_line(int16_t openmv_offset);
void Turn(int8_t TURN_FLAG);
void Turn_back(void);

#endif
