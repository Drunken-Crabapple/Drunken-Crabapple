#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/MOTOR/motor.h"
#include "./BSP/TIMER/btim.h"
#include "./BSP/ENCODER/encoder.h"
#include "./BSP/EXTI/exti.h"
#include "./BSP/OPENMV/openmv.h"
#include "main.h"

const Pidparams_t pid_params_motor1 = 
{
    .kp = 0.31f,
    .ki = 0.1f,
    .kd = 0.002f,
    .out_min = -80.0f,
    .out_max = 80.0f,
};
const Pidparams_t pid_params_motor2 =
{
    .kp = 0.31f,
    .ki = 0.1f,
    .kd = 0.002f,
    .out_min = -80.0f,
    .out_max = 80.0f,
};

PID_t pid_motor1;
PID_t pid_motor2;

int8_t TURN_FLAG = 1;      //左右转标志位，-1右转 +1左转   
uint32_t turn_start_tick = 0;


uint8_t turn_done = 0;
uint8_t turning = 0;        //转弯标志位
uint8_t stop_done = 0;
uint8_t back_done = 0;
uint8_t backing = 0;
uint32_t back_start_tick = 0;

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    usart_init(115200);
    openmv_uart2_init(115200);
    exti_init();

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
    
    btim_tim6_int_init(8400 - 1,10 - 1);        //1ms

    while(1)
    {
        openmv_parse_data();
        if(openmv_data_ready)       //已经接收完毕数据并完成数据解析
        {
            openmv_data_ready = 0;
            
            if((turn_done == 1) && (stop_done == 1) && (back_done == 0) && (backing == 0))  //还未回转
            {
                backing = 1;                                    //回转开始
                back_start_tick = HAL_GetTick();
            }

            if((openmv_state == 2) && (turn_done == 0) && (turning == 0))   //第一次十字路口,先固定左转
            {
                turning = 1;        //转弯标志
                turn_start_tick = HAL_GetTick();
                turn_delta = 0.0f;
            }

            if(backing == 1)            //回转
            {
                if((openmv_state == 1) && (HAL_GetTick() - back_start_tick > 600))
                {
                    backing = 0;
                    back_done = 1;
                    stop_done = 0;
                    
                    turning = 2;        //第二次转十字
                    pid_reset(&pid_motor1);
                    pid_reset(&pid_motor2);
                    Direct_line(openmv_offset);
                    //pid_set_target(&pid_motor1,40.0f);
                    //pid_set_target(&pid_motor2,40.0f);
                }
                else
                {
                    Turn_back();
                }
            }
            if((turning == 2) && (openmv_state == 2))
            {   
                turning = 1;            //第二次转十字
                TURN_FLAG *= -1;
                turn_start_tick = HAL_GetTick();
            }

            else if(turning == 1)             //转直角弯
            {
                if((openmv_state == 1) && (HAL_GetTick() - turn_start_tick > 600))  //转弯完成后判定为直线
                {
                    turning = 0;              //第一次转弯结束
                    turn_delta = 0.0f;
                    turn_done = 1;
                    turn_start_tick = 0.0f;     //重置转弯计时
                    Direct_line(openmv_offset);
                }
                else                                                                //正在转弯
                {
                    Turn(TURN_FLAG);
                }
            }
            else                            //既不是回转，也不是直角弯，直走                                                   
            {
                if(openmv_state == 1)                                               //还没到转弯，正常直行，或者转弯完成之后继续巡线
                {
                    Direct_line(openmv_offset);
                }
                else if((openmv_state == 0) && (backing == 0))                                          //停
                {
                    pid_set_target(&pid_motor1, 0.0f);
                    pid_set_target(&pid_motor2, 0.0f);
                    if(turn_done)
                    {
                        stop_done = 1;      //第一次停
                    }
                }
            }                    
            
            printf("openmv:%d,%d,%f,%f,%f,%f\r\n",openmv_state,openmv_offset,pid_motor1.target,pid_motor1.current,pid_motor2.target,pid_motor2.current);
            
        }   
        
        if(flag >= 40)
        {   
            flag = 0;
            motor1_set_speed(pid_calculate(&pid_motor1,(float)motor1_encoder_speed_update()));
            motor2_set_speed(pid_calculate(&pid_motor2,(float)motor2_encoder_speed_update()));
            //printf("%f,%f,%f,%f\r\n",pid_motor1.target,pid_motor1.current,pid_motor2.target,pid_motor2.current);
        }   
    }
}
