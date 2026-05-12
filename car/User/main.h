#ifndef __MAIN_H
#define __MAIN_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/PID/pid.h"
#include "./BSP/MOVE/move.h"

#define MOTOR_TARGET        70.0f / 60.0f / 1000.0f * 40.0f * 20.0f * 13.0f * 4.0f   //  rpm / 1.44230769
#define TURN_TARGET         70.0f / 60.0f / 1000.0f * 40.0f * 20.0f * 13.0f * 4.0f
#define TEN_RPM             5.0f / 60.0f / 1000.0f * 40.0f * 20.0f * 13.0f * 4.0f
#define BACK_RPM            75.0f / 60.0f / 1000.0f * 40.0f * 20.0f * 13.0f * 4.0f
#define OPENMV_TURN_KP      0.5f
#define OPENMV_TURN_LIMIT   (MOTOR_TARGET * 0.9f)


extern PID_t pid_motor1;
extern PID_t pid_motor2;
extern const Pidparams_t pid_params_motor1;
extern const Pidparams_t pid_params_motor2;

#endif
