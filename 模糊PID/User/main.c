#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/MOTOR/motor.h"
#include "./BSP/TIMER/btim.h"
#include "./BSP/ENCODER/encoder.h"
#include "./BSP/PID/pid.h"
#include "./BSP/PID/fuzzy_pid.h"
#include "./BSP/EXTI/exti.h"
#include "main.h"

const Pidparams_t pid_params_motor = 
{
    .kp = 4.0f,
    .ki = 0.02f,
    .kd = 0.04f,
    .out_min = -3500.0f,
    .out_max = 3500.0f,
    .integral_min = -1000.0f,
    .integral_max = 1000.0f,
};



PID_t pid_motor1;
PID_t pid_motor2;

FuzzyPID_t fuzzy_motor1;
FuzzyPID_t fuzzy_motor2;

int main(void)
{
    float ke;
    float kec;


    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    usart_init(115200);
    exti_init();

    motor1_tim9_pwm_init(2 - 1,4200 - 1);       //20kHz
    motor2_tim12_pwm_init(1 - 1,4200 - 1);      //20kHz
    motor1_set_speed(0);
    motor2_set_speed(0);
    encoder1_tim3_init();
    encoder2_tim4_init();

    pid_init(&pid_motor1,&pid_params_motor);
    pid_init(&pid_motor2,&pid_params_motor);

    pid_set_target(&pid_motor1,MOTOR_TARGET_RPM);
    pid_set_target(&pid_motor2,MOTOR_TARGET_RPM);

    ke = 3.0f / FUZZY_E_MAX_RPM;
    kec = 3.0f / FUZZY_EC_MAX_RPM;

    fuzzy_pid_init(&fuzzy_motor1,ke,kec,FUZZY_KP_OUT_GAIN,FUZZY_KI_OUT_GAIN,FUZZY_KD_OUT_GAIN);
    fuzzy_pid_init(&fuzzy_motor2,ke,kec,FUZZY_KP_OUT_GAIN,FUZZY_KI_OUT_GAIN,FUZZY_KD_OUT_GAIN);


    btim_tim6_int_init(8400 - 1,50 - 1);

    while(1)
    {
        
    }
}
