#ifndef __IMUX_H
#define __IMUX_H

#include "./SYSTEM/sys/sys.h"
#include "main.h"

void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az) ;
void Get_Eulerian_Angle(_out_angle *Angle);

#endif
