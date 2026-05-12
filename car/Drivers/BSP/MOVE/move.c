#include "./BSP/MOVE/move.h"
#include "main.h"
#include "./BSP/PID/pid.h"
#include "./BSP/MOTOR/motor.h"

float turn_delta = 0.0f;
float turn = 0.0f;
float base_target = 0.0f;

void Direct_line(int16_t openmv_offset)
{
    base_target = MOTOR_TARGET;
    turn = openmv_offset * OPENMV_TURN_KP;

    if(turn > OPENMV_TURN_LIMIT)  turn = OPENMV_TURN_LIMIT;
    if(turn < -OPENMV_TURN_LIMIT) turn = -OPENMV_TURN_LIMIT;

    pid_set_target(&pid_motor1, base_target + turn);
    pid_set_target(&pid_motor2, base_target - turn);
}

void Turn(int8_t TURN_FLAG)
{
    turn_delta += TEN_RPM;
                    
    if(turn_delta > OPENMV_TURN_LIMIT)
    {
        turn_delta = OPENMV_TURN_LIMIT;
    }

    pid_set_target(&pid_motor1, MOTOR_TARGET - TURN_FLAG * turn_delta);
    pid_set_target(&pid_motor2, MOTOR_TARGET + TURN_FLAG * turn_delta); 
}

void Turn_back(void)
{
    float back_target = MOTOR_TARGET;
    pid_set_target(&pid_motor1,-back_target);
    pid_set_target(&pid_motor2,back_target);
    //motor1_set_speed(-BACK_RPM);
    //motor2_set_speed(BACK_RPM);
}
