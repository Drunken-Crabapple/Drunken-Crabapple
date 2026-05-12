#include "./BSP/MOVE/move.h"
#include "main.h"
#include "./BSP/PID/pid.h"
#include "./BSP/MOTOR/motor.h"
#include "./BSP/ENCODER/encoder.h"

float turn_delta = 0.0f;
float turn = 0.0f;
float base_target = 0.0f;

typedef enum
{
    CAR_OUTBOUND_LINE = 0,      //去直
    CAR_OUTBOUND_TURN,          //转弯
    CAR_OUTBOUND_AFTER_TURN,    //转弯结束标
    CAR_TURN_BACK,              //回转
    CAR_RETURN_LINE,            //回转结束走直线
    CAR_RETURN_TURN,            //返回转弯
    CAR_RETURN_AFTER_TURN,      //结束标
    CAR_FINISH_STOP             //停
}CarMode_t;


static CarMode_t car_mode = CAR_OUTBOUND_LINE;

static uint8_t last_openmv_state = 0;
static uint8_t first_turn_index = 2;
static int8_t go_turn_flag = 0;
static uint8_t cross_count = 0;

static uint32_t turn_start_tick = 0;
static uint32_t back_start_tick = 0;

/*
    电机，编码器，pid控制器初始化
*/
void motion_init(void)
{
    motor1_tim9_pwm_init(168 - 1,100 - 1);       //10kHz
    motor2_tim12_pwm_init(84 - 1,100 - 1);      //10kHz
    motor1_set_speed(0);
    motor2_set_speed(0);
    encoder1_tim3_init();
    encoder2_tim4_init();
    pid_reset(&pid_motor1);
    pid_reset(&pid_motor2);

    pid_init(&pid_motor1,&pid_params_motor1);
    pid_init(&pid_motor2,&pid_params_motor2);
    
    pid_set_target(&pid_motor1,0.0f);
    pid_set_target(&pid_motor2,0.0f);
}

void Direct_line(int16_t openmv_offset)
{
    base_target = MOTOR_TARGET;
    turn = openmv_offset * OPENMV_TURN_KP;

    if(turn > OPENMV_TURN_LIMIT)  turn = OPENMV_TURN_LIMIT;
    if(turn < -OPENMV_TURN_LIMIT) turn = -OPENMV_TURN_LIMIT;

    pid_set_target(&pid_motor1, base_target + turn);
    pid_set_target(&pid_motor2, base_target - turn);
}

/*
    转弯驱动函数， 调整TURN_FLAG调整转向
*/
void Turn(int8_t TURN_FLAG)
{
    turn_delta += TEN_RPM;
                    
    if(turn_delta > OPENMV_TURN_LIMIT)
    {
        turn_delta = OPENMV_TURN_LIMIT;
    }

    pid_set_target(&pid_motor1, TURN_TARGET - TURN_FLAG * turn_delta);
    pid_set_target(&pid_motor2, TURN_TARGET + TURN_FLAG * turn_delta); 
}

/*
    回转驱动函数
*/
void Turn_back(void)
{
    float back_target = MOTOR_TARGET;
    pid_set_target(&pid_motor1,-back_target);
    pid_set_target(&pid_motor2,back_target);
}


/*
    判断是否是十字路口,只有当当前openmv_state == 2且当前openmv_state与上一时刻不一致才计数
*/
static uint8_t is_new_cross(uint8_t openmv_state)
{
    uint8_t ret = 0;

    if((openmv_state == 2) && (last_openmv_state != 2))
    {
        ret = 1;
    }

    last_openmv_state = openmv_state;
    return ret;
}

/*
    回转时清空pid控制器状态，用的是增量式pid，如果不清空状态，回转时容易转过头很难寻到线
*/
static void motion_reset_pid(void)
{
    pid_reset(&pid_motor1);
    pid_reset(&pid_motor2);
}

/*
    识别到state == 0时停止，同时清空pid控制器状态
*/
static void motion_stop(void)
{
    pid_reset(&pid_motor1);
    pid_reset(&pid_motor2);

    pid_set_target(&pid_motor1,0.0f);
    pid_set_target(&pid_motor2,0.0f);

    motor1_set_speed(0);
    motor2_set_speed(0);
}

/*
first_turn_cross_index:第几个十字路口转弯
turn_flag:  +1->左转    -1->右转
*/
void Car_Control_Init(uint8_t first_turn_cross_index,int8_t turn_flag)
{
    car_mode = CAR_OUTBOUND_LINE;

    first_turn_index = first_turn_cross_index;
    go_turn_flag = turn_flag;

    cross_count = 0;
    last_openmv_state = 0;

    turn_start_tick = 0;
    back_start_tick = 0;

    turn_delta = 0.0f;

    motion_stop();      //确保初始化后车是停的
}

/*
    1.判断当前是否是新十字路口
    2.根据car_mode的当前阶段决定动作
*/
void Car_Control_Update(uint8_t openmv_state,int16_t openmv_offset)
{
    uint8_t new_cross = is_new_cross(openmv_state);
    
    switch (car_mode)
    {
        case CAR_OUTBOUND_LINE:
            if(new_cross)
            {
                cross_count++;

                if(cross_count >= first_turn_index)
                {
                    cross_count = 0;
                    car_mode = CAR_OUTBOUND_TURN;
                    turn_start_tick = HAL_GetTick();
                    turn_delta = 0.0f;
                    //motion_reset_pid();
                    Turn(go_turn_flag);
                    break;
                }
            }
            
            if(openmv_state == 0)
            {
                motion_stop();
            }

            else
            {
                Direct_line(openmv_offset);
            }
            break;
            
        case CAR_OUTBOUND_TURN:
            if((openmv_state == 1) && (HAL_GetTick() - turn_start_tick > 600))
            {
                car_mode = CAR_OUTBOUND_AFTER_TURN;
                turn_delta = 0.0f;
                //motion_reset_pid();
                Direct_line(openmv_offset);
            }
            else
            {
                Turn(go_turn_flag);
            }
            break;

        case CAR_OUTBOUND_AFTER_TURN:
            if(openmv_state == 0)
            {
                motion_stop();

                car_mode = CAR_TURN_BACK;
                back_start_tick = HAL_GetTick();
                turn_delta = 0.0f;
            }
            else
            {
                Direct_line(openmv_offset);
            }
            break;

        case CAR_TURN_BACK:
            if((openmv_state == 1) && (HAL_GetTick() - back_start_tick > 600))
            {
                car_mode = CAR_RETURN_LINE;
                turn_delta = 0.0f;
                motion_reset_pid();
                Direct_line(openmv_offset);
            }
            else
            {
                Turn_back();
            }
            break;
    
        case CAR_RETURN_LINE:
            if(new_cross)
            {
                cross_count++;

                if(cross_count == 1)
                {
                    car_mode = CAR_RETURN_TURN;
                    turn_start_tick = HAL_GetTick();
                    turn_delta = 0.0f;
                    //motion_reset_pid();
                    Turn(-go_turn_flag);
                    break;
                }
            }

            if(openmv_state == 0)
            {
                motion_stop();
                car_mode = CAR_FINISH_STOP;
            }
            else
            {
                Direct_line(openmv_offset);
            }
            break;

        case CAR_RETURN_TURN:
            
            if((openmv_state == 1) && (HAL_GetTick() - turn_start_tick > 600))
            {
                car_mode = CAR_RETURN_AFTER_TURN;
                turn_delta = 0.0f;
                //motion_reset_pid();
                Direct_line(openmv_offset);
            }
            else
            {
                Turn(-go_turn_flag);
            }
            break;

        case CAR_RETURN_AFTER_TURN:
            if(openmv_state == 0)
            {
                motion_stop();
                car_mode = CAR_FINISH_STOP;
            }
            else
            {
                Direct_line(openmv_offset);
            }
            break;

        case CAR_FINISH_STOP:

        default:
            motion_stop();
            break;
    }
}
