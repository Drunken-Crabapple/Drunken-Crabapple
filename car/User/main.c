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

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz  n m p q*/
    delay_init(168);                    /* 延时初始化 */
    usart_init(115200);
    openmv_uart2_init(115200);
    exti_init();

    motion_init();
    Car_Control_Init(1,-51);          //第二个路口转弯，左转
    
    btim_tim6_int_init(8400 - 1,10 - 1);        //1ms

    while(1)
    {
        openmv_parse_data();
        if(openmv_data_ready)       //已经接收完毕数据并完成数据解析
        {
            openmv_data_ready = 0;
            
            Car_Control_Update(openmv_state,openmv_offset);
            
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
